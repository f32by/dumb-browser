solutions = [
  {
    "name": "src",
    "url": "https://chromium.googlesource.com/chromium/src.git",
    "managed": False,
    "custom_deps": {
      "src/third_party/WebKit/LayoutTests": None,
      "src/chrome_frame/tools/test/reference_build/chrome": None,
      "src/chrome_frame/tools/test/reference_build/chrome_win": None,
      "src/chrome/tools/test/reference_build/chrome": None,
      "src/chrome/tools/test/reference_build/chrome_linux": None,
      "src/chrome/tools/test/reference_build/chrome_mac": None,
      "src/chrome/tools/test/reference_build/chrome_win": None,
      "src/chrome/test/data/autofill/captured_sites": None,
      "src/chrome/test/data/password/captured_sites": None,
      "src/chrome/test/data/perf/canvas_bench": None,
      "src/chrome/test/data/perf/frame_rate/content": None,
      "src/chrome/test/data/safe_browsing/dmg": None,
      "src/chrome/test/data/xr/webvr_info": None,
    },
    "custom_vars": {
      "checkout_nacl": False,
      "checkout_pgo_profiles": True
    }
  },
]
