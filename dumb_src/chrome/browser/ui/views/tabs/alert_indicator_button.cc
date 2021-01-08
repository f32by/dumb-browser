// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/tabs/alert_indicator_button.h"

#include <utility>

#include "base/command_line.h"
#include "chrome/app/vector_icons/vector_icons.h"
#include "chrome/browser/ui/layout_constants.h"
#include "chrome/browser/ui/tabs/tab_utils.h"
#include "chrome/browser/ui/views/tabs/tab.h"
#include "chrome/browser/ui/views/tabs/tab_controller.h"
#include "chrome/common/chrome_switches.h"
#include "components/vector_icons/vector_icons.h"
#include "ui/base/pointer/touch_ui_controller.h"
#include "ui/gfx/animation/multi_animation.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/paint_vector_icon.h"
#include "ui/views/animation/animation_delegate_views.h"
#include "ui/views/metrics.h"

namespace {

// The minimum required click-to-select area of an inactive Tab before allowing
// the click-to-mute functionality to be enabled.  These values are in terms of
// some percentage of the AlertIndicatorButton's width.  See comments in
// UpdateEnabledForMuteToggle().
const int kMinMouseSelectableAreaPercent = 250;
const int kMinGestureSelectableAreaPercent = 400;

// Fade-in/out duration for the tab indicator animations.  Fade-in is quick to
// immediately notify the user.  Fade-out is more gradual, so that the user has
// a chance of finding a tab that has quickly "blipped" on and off.
constexpr auto kIndicatorFadeInDuration =
    base::TimeDelta::FromMilliseconds(200);
constexpr auto kIndicatorFadeOutDuration =
    base::TimeDelta::FromMilliseconds(1000);

// Interval between frame updates of the tab indicator animations.  This is not
// the usual 60 FPS because a trade-off must be made between tab UI animation
// smoothness and media recording/playback performance on low-end hardware.
constexpr base::TimeDelta kIndicatorFrameInterval =
    base::TimeDelta::FromMilliseconds(50);  // 20 FPS

// Returns true if either Shift or Control are being held down.  In this case,
// mouse events are delegated to the Tab, to perform tab selection in the tab
// strip instead.
bool IsShiftOrControlDown(const ui::Event& event) {
  return (event.flags() & (ui::EF_SHIFT_DOWN | ui::EF_CONTROL_DOWN)) != 0;
}

// Animation that throbs in (towards 1.0) and out (towards 0.0), and ends in the
// "in" state.
class TabRecordingIndicatorAnimation : public gfx::MultiAnimation {
 public:
  TabRecordingIndicatorAnimation(const gfx::MultiAnimation::Parts& parts,
                                 const base::TimeDelta interval)
      : MultiAnimation(parts, interval) {}
  ~TabRecordingIndicatorAnimation() override = default;

  // Overridden to provide alternating "towards in" and "towards out" behavior.
  double GetCurrentValue() const override;

  static std::unique_ptr<TabRecordingIndicatorAnimation> Create();
};

double TabRecordingIndicatorAnimation::GetCurrentValue() const {
  return current_part_index() % 2 ? 1.0 - MultiAnimation::GetCurrentValue()
                                  : MultiAnimation::GetCurrentValue();
}

std::unique_ptr<TabRecordingIndicatorAnimation>
TabRecordingIndicatorAnimation::Create() {
  // Number of times to "toggle throb" the recording and tab capture indicators
  // when they first appear.
  constexpr size_t kCaptureIndicatorThrobCycles = 5;

  MultiAnimation::Parts parts;
  static_assert(
      kCaptureIndicatorThrobCycles % 2 != 0,
      "odd number of cycles required so animation finishes in showing state");
  for (size_t i = 0; i < kCaptureIndicatorThrobCycles; ++i) {
    parts.push_back(MultiAnimation::Part(
        i % 2 ? kIndicatorFadeOutDuration : kIndicatorFadeInDuration,
        gfx::Tween::EASE_IN));
  }

  auto animation = std::make_unique<TabRecordingIndicatorAnimation>(
      parts, kIndicatorFrameInterval);
  animation->set_continuous(false);
  return animation;
}

// Returns a non-continuous Animation that performs a fade-in or fade-out
// appropriate for the given |next_alert_state|.  This is used by the tab alert
// indicator to alert the user that recording, tab capture, or audio playback
// has started/stopped.
std::unique_ptr<gfx::Animation> CreateTabAlertIndicatorFadeAnimation(
    base::Optional<TabAlertState> alert_state) {
  if (alert_state == TabAlertState::MEDIA_RECORDING ||
      alert_state == TabAlertState::TAB_CAPTURING ||
      alert_state == TabAlertState::DESKTOP_CAPTURING) {
    return TabRecordingIndicatorAnimation::Create();
  }

  // Note: While it seems silly to use a one-part MultiAnimation, it's the only
  // gfx::Animation implementation that lets us control the frame interval.
  gfx::MultiAnimation::Parts parts;
  const bool is_for_fade_in = (alert_state.has_value());
  parts.push_back(gfx::MultiAnimation::Part(
      is_for_fade_in ? kIndicatorFadeInDuration : kIndicatorFadeOutDuration,
      gfx::Tween::EASE_IN));
  auto animation =
      std::make_unique<gfx::MultiAnimation>(parts, kIndicatorFrameInterval);
  animation->set_continuous(false);
  return std::move(animation);
}

// Returns a cached image, to be shown by the alert indicator for the given
// |alert_state|.  Uses the global ui::ResourceBundle shared instance.
gfx::Image GetTabAlertIndicatorImage(TabAlertState alert_state,
                                     SkColor button_color) {
  const gfx::VectorIcon* icon = nullptr;
  int image_width = GetLayoutConstant(TAB_ALERT_INDICATOR_ICON_WIDTH);
  const bool touch_ui = ui::TouchUiController::Get()->touch_ui();
  switch (alert_state) {
    case TabAlertState::AUDIO_PLAYING:
      icon = touch_ui ? &kTabAudioRoundedIcon : &kTabAudioIcon;
      break;
    case TabAlertState::AUDIO_MUTING:
      icon = touch_ui ? &kTabAudioMutingRoundedIcon : &kTabAudioMutingIcon;
      break;
    case TabAlertState::MEDIA_RECORDING:
    case TabAlertState::DESKTOP_CAPTURING:
      icon = &kTabMediaRecordingIcon;
      break;
    case TabAlertState::TAB_CAPTURING:
      icon =
          touch_ui ? &kTabMediaCapturingWithArrowIcon : &kTabMediaCapturingIcon;
      // Tab capturing and presenting icon uses a different width compared to
      // the other tab alert indicator icons.
      image_width = GetLayoutConstant(TAB_ALERT_INDICATOR_CAPTURE_ICON_WIDTH);
      break;
    case TabAlertState::BLUETOOTH_CONNECTED:
      icon = &kTabBluetoothConnectedIcon;
      break;
    case TabAlertState::BLUETOOTH_SCAN_ACTIVE:
      icon = &kTabBluetoothScanActiveIcon;
      break;
    case TabAlertState::USB_CONNECTED:
      icon = &kTabUsbConnectedIcon;
      break;
    case TabAlertState::HID_CONNECTED:
      icon = &vector_icons::kVideogameAssetIcon;
      break;
    case TabAlertState::SERIAL_CONNECTED:
      // TODO(https://crbug.com/917204): This icon is too large to fit properly
      // as a tab indicator and should be replaced.
      icon = &vector_icons::kSerialPortIcon;
      break;
    case TabAlertState::PIP_PLAYING:
      icon = &kPictureInPictureAltIcon;
      break;
    case TabAlertState::VR_PRESENTING_IN_HEADSET:
      icon = &vector_icons::kVrHeadsetIcon;
      break;
  }
  DCHECK(icon);
  return gfx::Image(gfx::CreateVectorIcon(*icon, image_width, button_color));
}

gfx::Image GetTabAlertIndicatorAffordanceImage(TabAlertState alert_state,
                                               SkColor button_color) {
  switch (alert_state) {
    case TabAlertState::AUDIO_PLAYING:
      return GetTabAlertIndicatorImage(TabAlertState::AUDIO_MUTING,
                                       button_color);
    case TabAlertState::AUDIO_MUTING:
      return GetTabAlertIndicatorImage(TabAlertState::AUDIO_PLAYING,
                                       button_color);
    case TabAlertState::MEDIA_RECORDING:
    case TabAlertState::TAB_CAPTURING:
    case TabAlertState::BLUETOOTH_CONNECTED:
    case TabAlertState::BLUETOOTH_SCAN_ACTIVE:
    case TabAlertState::USB_CONNECTED:
    case TabAlertState::PIP_PLAYING:
    case TabAlertState::DESKTOP_CAPTURING:
    case TabAlertState::HID_CONNECTED:
    case TabAlertState::SERIAL_CONNECTED:
    case TabAlertState::VR_PRESENTING_IN_HEADSET:
      return GetTabAlertIndicatorImage(alert_state, button_color);
  }
  NOTREACHED();
  return GetTabAlertIndicatorImage(alert_state, button_color);
}

bool AreExperimentalMuteControlsEnabled() {
  return base::CommandLine::ForCurrentProcess()->HasSwitch(
      switches::kEnableTabAudioMuting);
}

}  // namespace

const char AlertIndicatorButton::kViewClassName[] = "AlertIndicatorButton";

class AlertIndicatorButton::FadeAnimationDelegate
    : public gfx::AnimationDelegate {
 public:
  explicit FadeAnimationDelegate(AlertIndicatorButton* button)
      : button_(button) {}
  ~FadeAnimationDelegate() override {}

 private:
  // gfx::AnimationDelegate
  void AnimationProgressed(const gfx::Animation* animation) override {
    button_->SchedulePaint();
  }

  void AnimationCanceled(const gfx::Animation* animation) override {
    AnimationEnded(animation);
  }

  void AnimationEnded(const gfx::Animation* animation) override {
    button_->showing_alert_state_ = button_->alert_state_;
    button_->SchedulePaint();
    button_->parent_tab_->AlertStateChanged();
  }

  AlertIndicatorButton* const button_;

  DISALLOW_COPY_AND_ASSIGN(FadeAnimationDelegate);
};

AlertIndicatorButton::AlertIndicatorButton(Tab* parent_tab)
    : views::ImageButton(nullptr),
      parent_tab_(parent_tab) {
  DCHECK(parent_tab_);
  SetEventTargeter(
      std::unique_ptr<views::ViewTargeter>(new views::ViewTargeter(this)));

  // Disable animations of hover state change, to be consistent with the
  // behavior of the tab close button.
  SetAnimateOnStateChange(false);
}

AlertIndicatorButton::~AlertIndicatorButton() {}

void AlertIndicatorButton::TransitionToAlertState(
    base::Optional<TabAlertState> next_state) {
  if (next_state == alert_state_)
    return;

  base::Optional<TabAlertState> previous_alert_showing_state =
      showing_alert_state_;

  if (next_state)
    ResetImages(next_state.value());

  if ((alert_state_ == TabAlertState::AUDIO_PLAYING &&
       next_state == TabAlertState::AUDIO_MUTING) ||
      (alert_state_ == TabAlertState::AUDIO_MUTING &&
       next_state == TabAlertState::AUDIO_PLAYING)) {
    // Instant user feedback: No fade animation.
    showing_alert_state_ = next_state;
    fade_animation_.reset();
  } else {
    if (!next_state)
      showing_alert_state_ = alert_state_;  // Fading-out indicator.
    else
      showing_alert_state_ = next_state;  // Fading-in to next indicator.
    fade_animation_ = CreateTabAlertIndicatorFadeAnimation(next_state);
    if (!fade_animation_delegate_)
      fade_animation_delegate_ = std::make_unique<FadeAnimationDelegate>(this);
    fade_animation_->set_delegate(fade_animation_delegate_.get());
    fade_animation_->Start();
  }

  alert_state_ = next_state;

  if (previous_alert_showing_state != showing_alert_state_)
    parent_tab_->AlertStateChanged();

  UpdateEnabledForMuteToggle();
}

void AlertIndicatorButton::UpdateEnabledForMuteToggle() {
  const bool was_enabled = GetEnabled();

  bool enable = AreExperimentalMuteControlsEnabled() &&
                (alert_state_ == TabAlertState::AUDIO_PLAYING ||
                 alert_state_ == TabAlertState::AUDIO_MUTING);

  // If the tab is not the currently-active tab, make sure it is wide enough
  // before enabling click-to-mute.  This ensures that there is enough click
  // area for the user to activate a tab rather than unintentionally muting it.
  // Note that IsTriggerableEvent() is also overridden to provide an even wider
  // requirement for tap gestures.
  if (enable && !GetTab()->IsActive()) {
    const int required_width = width() * kMinMouseSelectableAreaPercent / 100;
    enable = (GetTab()->GetWidthOfLargestSelectableRegion() >= required_width);
  }

  if (enable == was_enabled)
    return;

  SetEnabled(enable);

  // If the button has become enabled, check whether the mouse is currently
  // hovering.  If it is, enter a dormant period where extra user clicks are
  // prevented from having an effect (i.e., before the user has realized the
  // button has become enabled underneath their cursor).
  if (!was_enabled && GetState() == views::Button::STATE_HOVERED)
    EnterDormantPeriod();
  else if (!GetEnabled())
    ExitDormantPeriod();
}

void AlertIndicatorButton::OnParentTabButtonColorChanged() {
  if (alert_state_ == TabAlertState::AUDIO_PLAYING ||
      alert_state_ == TabAlertState::AUDIO_MUTING)
    ResetImages(alert_state_);
}

const char* AlertIndicatorButton::GetClassName() const {
  return kViewClassName;
}

views::View* AlertIndicatorButton::GetTooltipHandlerForPoint(
    const gfx::Point& point) {
  return nullptr;  // Tab (the parent View) provides the tooltip.
}

bool AlertIndicatorButton::OnMousePressed(const ui::MouseEvent& event) {
  // Do not handle this mouse event when anything but the left mouse button is
  // pressed or when any modifier keys are being held down.  Instead, the Tab
  // should react (e.g., middle-click for close, right-click for context menu).
  if (!event.IsOnlyLeftMouseButton() || IsShiftOrControlDown(event)) {
    if (GetState() != views::Button::STATE_DISABLED)
      SetState(views::Button::STATE_NORMAL);  // Turn off hover.
    return false;  // Event to be handled by Tab.
  }
  return ImageButton::OnMousePressed(event);
}

bool AlertIndicatorButton::OnMouseDragged(const ui::MouseEvent& event) {
  // const ButtonState previous_state = GetState();
  const bool ret = ImageButton::OnMouseDragged(event);
  return ret;
}

void AlertIndicatorButton::OnMouseEntered(const ui::MouseEvent& event) {
  // If any modifier keys are being held down, do not turn on hover.
  if (GetState() != views::Button::STATE_DISABLED && IsShiftOrControlDown(event)) {
    SetState(views::Button::STATE_NORMAL);
    return;
  }
  ImageButton::OnMouseEntered(event);
}

void AlertIndicatorButton::OnMouseExited(const ui::MouseEvent& event) {
  ExitDormantPeriod();
  ImageButton::OnMouseExited(event);
}

void AlertIndicatorButton::OnMouseMoved(const ui::MouseEvent& event) {
  // If any modifier keys are being held down, turn off hover.
  if (GetState() != views::Button::STATE_DISABLED && IsShiftOrControlDown(event)) {
    SetState(views::Button::STATE_NORMAL);
    return;
  }
  ImageButton::OnMouseMoved(event);
}

void AlertIndicatorButton::OnBoundsChanged(const gfx::Rect& previous_bounds) {
  UpdateEnabledForMuteToggle();
}

bool AlertIndicatorButton::DoesIntersectRect(const views::View* target,
                                             const gfx::Rect& rect) const {
  // If this button is not enabled, Tab (the parent View) handles all mouse
  // events.
  return GetEnabled() &&
         views::ViewTargeterDelegate::DoesIntersectRect(target, rect);
}

void AlertIndicatorButton::NotifyClick(const ui::Event& event) {
  EnterDormantPeriod();

  // Call TransitionToAlertState() to change the image, providing the user with
  // instant feedback.  In the very unlikely event that the mute toggle fails,
  // TransitionToAlertState() will be called again, via another code path, to
  // set the image to be consistent with the final outcome.
  if (alert_state_ == TabAlertState::AUDIO_PLAYING) {
    TransitionToAlertState(TabAlertState::AUDIO_MUTING);
  } else {
    DCHECK(alert_state_ == TabAlertState::AUDIO_MUTING);
    TransitionToAlertState(TabAlertState::AUDIO_PLAYING);
  }

  GetTab()->controller()->ToggleTabAudioMute(GetTab());
}

bool AlertIndicatorButton::IsTriggerableEvent(const ui::Event& event) {
  if (is_dormant())
    return false;

  // For mouse events, only trigger on the left mouse button and when no
  // modifier keys are being held down.
  if (event.IsMouseEvent() &&
      (!static_cast<const ui::MouseEvent*>(&event)->IsOnlyLeftMouseButton() ||
       IsShiftOrControlDown(event)))
    return false;

  // For gesture events on an inactive tab, require an even wider tab before
  // click-to-mute can be triggered.  See comments in
  // UpdateEnabledForMuteToggle().
  if (event.IsGestureEvent() && !GetTab()->IsActive()) {
    const int required_width = width() * kMinGestureSelectableAreaPercent / 100;
    if (GetTab()->GetWidthOfLargestSelectableRegion() < required_width)
      return false;
  }

  return views::ImageButton::IsTriggerableEvent(event);
}

void AlertIndicatorButton::PaintButtonContents(gfx::Canvas* canvas) {
  double opaqueness = 1.0;
  if (fade_animation_) {
    opaqueness = fade_animation_->GetCurrentValue();
    if (!alert_state_)
      opaqueness = 1.0 - opaqueness;  // Fading out, not in.
  } else if (is_dormant()) {
    opaqueness = 0.5;
  }
  if (opaqueness < 1.0)
    canvas->SaveLayerAlpha(opaqueness * SK_AlphaOPAQUE);
  ImageButton::PaintButtonContents(canvas);
  if (opaqueness < 1.0)
    canvas->Restore();
}

gfx::ImageSkia AlertIndicatorButton::GetImageToPaint() {
  if (is_dormant())
    return views::ImageButton::images_[views::Button::STATE_NORMAL];
  return views::ImageButton::GetImageToPaint();
}

Tab* AlertIndicatorButton::GetTab() const {
  DCHECK_EQ(static_cast<views::View*>(parent_tab_), parent());
  return parent_tab_;
}

void AlertIndicatorButton::ResetImages(base::Optional<TabAlertState> state) {
  SkColor color = parent_tab_->GetAlertIndicatorColor(state.value());
  gfx::ImageSkia indicator_image =
      GetTabAlertIndicatorImage(state.value(), color).AsImageSkia();
  SetImage(views::Button::STATE_NORMAL, &indicator_image);
  SetImage(views::Button::STATE_DISABLED, &indicator_image);
  gfx::ImageSkia affordance_image =
      GetTabAlertIndicatorAffordanceImage(state.value(), color).AsImageSkia();
  SetImage(views::Button::STATE_HOVERED, &affordance_image);
  SetImage(views::Button::STATE_PRESSED, &affordance_image);
}

void AlertIndicatorButton::EnterDormantPeriod() {
  wake_up_timer_.reset(new base::OneShotTimer());
  wake_up_timer_->Start(
      FROM_HERE,
      base::TimeDelta::FromMilliseconds(views::GetDoubleClickInterval()),
      this,
      &AlertIndicatorButton::ExitDormantPeriod);
  SchedulePaint();
}

void AlertIndicatorButton::ExitDormantPeriod() {
  const bool needs_repaint = is_dormant();
  wake_up_timer_.reset();
  if (needs_repaint)
    SchedulePaint();
}
