/* Copyright (c) 2021 The Dumb Browser Authors. All rights reserved.
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "dumb/components/privacy_guard/url_purify/url_purify_default_rules.h"

#include "base/no_destructor.h"
#include "base/strings/string_util.h"

const URLPurifyRule& GetDefaultGlobalRules() {
  static base::NoDestructor<URLPurifyRule> rule(
    URLPurifyRule(
    "Global",
    ".*",
    {"utm(?:_[a-z_]*)?", "ga_[a-z_]+", "yclid", "_openstat",
     "fb_action_(?:types|ids)", "fb_(?:source|ref)", "fbclid",
     "action_(?:object|type|ref)_map", "gs_l", "mkt_tok",
     "hmb_(?:campaign|medium|source)", "ref_?", "referrer",
     "gclid", "otm_[a-z_]*", "cmpid", "os_ehash", "_ga",
     "__twitter_impression", "wt_?z?mc", "wtrid", "[a-z]?mc",
     "dclid", "Echobox", "spm", "vn(?:_[a-z]*)+", "tracking_source", "from_spm_id"},
    base::nullopt
  ));

  return *rule.get();
}

const std::vector<URLPurifyRule>& GetDefaultPerSiteRules() {
  static std::vector<URLPurifyRule> rules {
    // Google
    {
      "Google",
      "(https:\\/\\/|http:\\/\\/)([a-zA-Z0-9-]*\\.)?(google)(\\.[a-zA-Z]{2,})(.*\\?.*)",
      {"ved", "bi[a-zA-Z]", "gfe_[a-zA-Z]", "ei", "source",
       "gs_[a-zA-Z]", "site", "oq", "esrc", "uact", "cd", "cad",
       "gws_[a-zA-Z]", "atyp", "vet", "zx", "_u", "je", "dcr",
       "ie", "sei", "sa", "dpr", "hl", "btn[a-zA-Z]", "sa",
       "usg", "cd", "cad", "uact"},
      {
        // Exclude Google video server
       ".*(.googlevideo.com).*",
       ".*(mail\\.google\\.).*(\\/mail\\/u\\/0).*",
       ".*(google\\.).*(\\/upload)?(\\/drive)\\/.*",
       ".*(docs\\.google\\.).*\\/.*",
       ".*(accounts\\.google\\.).*",
       ".*(google\\.).*\\/searchbyimage\\?image_url=[^\\/|\\?|&]*(\\/|&(amp;)?)?",
       ".*(hangouts\\.google\\.).*\\/webchat.*zx=[^\\/|\\?|&]*(\\/|&(amp;)?)?",
       ".*(client-channel\\.google\\.).*\\/client-channel.*zx=[^\\/|\\?|&]*(\\/|&(amp;)?)?",
       ".*(google\\.).*\\/complete\\/search\\?.*gs_[a-zA-Z]*=[^\\/|\\?|&]*(\\/|&(amp;)?)?",
       ".*(google\\.).*\\/s\\?tbm=map.*gs_[a-zA-Z]*=[^\\/|\\?|&]*(\\/|&(amp;)?)?",
       ".*(news\\.google\\.).*\\?hl=.*",
       ".*(google\\.).*\\/setprefs\\?.*hl=[^\\/|\\?|&]*(\\/|&(amp;)?)?",
       ".*(google\\.).*\\/appsactivity\\/.*",
       ".*(google\\.).*\\/recaptcha\\/.*"}
    },
    // Google Search
    {
      "Google Search",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?google(?:\\.[a-z]{2,}){1,}\\/search\\?",
      {"client", "sclient"},
      base::nullopt
    },
    // Amazon
    {
      "Amazon",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?amazon(?:\\.[a-z]{2,}){1,}",
      {"p[fd]_rd_[a-z]*", "qid", "srs?", "__mk_[a-z]{1,3}_[a-z]{1,3}",
       "spIA", "ms3_c", "[a-z%0-9]*ie", "refRID", "colii?d",
       "[^a-z%0-9]adId", "qualifier", "_encoding", "smid",
       "field-lbr_brands_browse-bin", "ref_?", "th", "sprefix",
       "crid", "keywords", "cv_ct_[a-z]+", "linkCode", "creativeASIN",
       "ascsubtag", "aaxitk", "hsa_cr_id", "sb-ci-[a-z]+", "rnid",
       "dchild", "camp", "creative", "s"},
      {"^https?:\\/\\/(?:[a-z0-9-]+\\.)*?amazon(?:\\.[a-z]{2,}){1,}\\/gp\\/.*?(?:redirector.html|cart\\/ajax-update.html|video\\/api\\/)",
       "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?amazon(?:\\.[a-z]{2,}){1,}\\/(?:hz\\/reviews-render\\/ajax\\/|message-us\\?|s\\?)"}
    },
    // Amazon Search
    {
      "Amazon Search",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?amazon(?:\\.[a-z]{2,}){1,}\\/s\\?",
      {"p[fd]_rd_[a-z]*", "qid", "srs?", "__mk_[a-z]{1,3}_[a-z]{1,3}",
      "spIA", "ms3_c", "[a-z%0-9]*ie", "refRID", "colii?d", "[^a-z%0-9]adId",
      "qualifier", "_encoding", "smid", "field-lbr_brands_browse-bin",
      "ref_?", "th", "sprefix", "crid", "keywords", "cv_ct_[a-z]+",
      "linkCode", "creativeASIN", "ascsubtag", "aaxitk", "hsa_cr_id",
      "sb-ci-[a-z]+", "rnid", "dchild", "camp", "creative"},
      base::nullopt
    },
    // Youtube
    {
      "Youtube",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?youtube\\.com",
      {"feature", "gclid", "kw"},
      base::nullopt
    },
    // Facebook
    {
      "Facebook",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?facebook\\.com",
      {"hc_[a-z_%\\[\\]0-9]*", "[a-z]*ref[a-z]*", "__tn__", "eid",
       "__xts__(?:\\[|%5B)\\d(?:\\]|%5D)", "comment_tracking", "dti",
       "app", "video_source", "ftentidentifier", "pageid", "padding",
       "ls_ref", "action_history"},
      {"^https?:\\/\\/(?:[a-z0-9-]+\\.)*?facebook\\.com\\/(plugins|ajax)\\/",
       "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?facebook\\.com\\/dialog\\/(?:share|send)",
       "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?facebook\\.com\\/groups\\/member_bio\\/bio_dialog\\/",
       "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?facebook\\.com\\/photo\\.php\\?",
       "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?facebook\\.com\\/privacy\\/specific_audience_selector_dialog\\/",
       "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?facebook\\.com\\/photo\\/download\\/"}
    },
    // Instagram
    {
      "Instagram",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?instagram\\.com",
      {"igshid"},
      base::nullopt
    },
    // Steampowered
    {
      "Steam",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?steampowered\\.com",
      {"snr"},
      base::nullopt
    },
    // GitHub
    {
      "GitHub",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?github\\.com",
      {"email_token", "email_source"},
      base::nullopt
    },
    // LinkedIn
    {
      "LinkedIn",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?linkedin\\.com",
      {"refId", "trk", "li[a-z]{2}"},
      base::nullopt
    },
    // lazada.com.my
    {
      "Lazada",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?lazada\\.com\\.my",
      {"ad_src", "did", "pa", "mp", "impsrc", "cid", "pos"},
      base::nullopt
    },
    // disq.us
    {
      "Disq.us",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?disq\\.us",
      {"cuid"},
      base::nullopt
    },
    // Mozilla
    {
      "Mozilla",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?mozilla\\.org",
      {"src", "platform", "redirect_source"},
      {"^https?:\\/\\/(?:[a-z0-9-]+\\.)*?mozilla.org\\/api"}
    },
    // New York Times
    {
      "New York Times",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?nytimes\\.com",
      {"smid"},
      base::nullopt
    },
    // Twitter
    {
      "Times",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?twitter.com",
      {"(?:ref_?)?src", "s", "cn", "ref_url"},
      base::nullopt
    },
    // Reddit
    {
      "Reddit",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?reddit.com",
      {"%24deep_link", "\\$deep_link", "correlation_id", "ref_campaign",
       "ref_source", "%243p", "\\$3p", "%24original_url", "\\$original_url",
       "_branch_match_id"},
      base::nullopt
    },
    // Netflix
    {
      "Netflix",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?netflix.com",
      {"trackId", "tctx", "jb[a-z]*?"},
      base::nullopt
    },
    // TechCrunch
    {
      "TechCrunch",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?techcrunch\\.com",
      {"ncid", "sr", "sr_share"},
      base::nullopt
    },
    // Bing
    {
      "Bing",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?bing(?:\\.[a-z]{2,}){1,}",
      {"cvid", "form", "sk", "sp", "sc", "qs", "qp"},
      {"^https?:\\/\\/(?:[a-z0-9-]+\\.)*?bing(?:\\.[a-z]{2,}){1,}\\/WS\\/redirect\\/"}
    },
    // Twitch
    {
      "Twitch",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?twitch\\.com",
      {"tt_medium", "tt_content"},
      base::nullopt
    },
    // IMDB
    {
      "IMDB",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?imdb\\.com",
      {"ref_", "pf_rd_[a-z]*"},
      base::nullopt
    },
    // Wallmart
    {
      "Wallmart",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?walmart\\.com",
      {"u1", "ath[a-z]*"},
      base::nullopt
    },
    // Youku
    {
      "Youku",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?youku\\.com",
      {"tpa"},
      base::nullopt
    },
    // Taobao
    {
      "Taobao",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?taobao\\.com",
      {"price", "sourceType", "suid", "ut_sk", "un", "share_crt_v", "sp_tk",
       "cpp", "shareurl", "short_name", "app", "scm[_a-z-]*", "pvid",
       "algo_expid", "algo_pvid", "ns", "abbucket", "ali_refid", "ali_trackid",
       "acm", "utparam", "pos", "abtest", "trackInfo", "utkn", "scene",
       "mytmenu", "turing_bucket", "lygClk", "impid", "bftTag", "bftRwd", "spm",
       "initiative_id", "sourceId", "ssid", "commend", "shareUniqueId",
       "bxsign", "app_pvid", "ptl", "union_lens" },
      base::nullopt
    },
    // Tmall
    {
      "Tmall",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?tmall\\.com",
      {"price", "sourceType", "suid", "ut_sk", "un", "share_crt_v", "sp_tk",
       "cpp", "shareurl", "short_name", "app", "scm[_a-z-]*", "pvid",
       "algo_expid", "algo_pvid", "ns", "abbucket", "ali_refid", "ali_trackid",
       "acm", "utparam", "pos", "abtest", "trackInfo", "user_number_id",
       "utkn", "scene", "mytmenu", "turing_bucket", "lygClk", "impid", "bftTag",
       "bftRwd", "activity_id", "from", "areaId", "user_id", "cat_id", "is_b",
       "rn"},
      base::nullopt
    },
    // tb.cn
    {
      "tb.cn",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?tb\\.cn",
      {"sm"},
      base::nullopt
    },
    // Aliexpress
    {
      "Aliexpress",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?aliexpress(?:\\.[a-z]{2,}){1,}",
      {"ws_ab_test", "btsid", "algo_expid", "algo_pvid", "gps-id",
       "scm[_a-z-]*", "cv", "af", "mall_affr", "sk", "dp", "terminal_id",
       "aff_request_id"},
      base::nullopt
    },
    // Bilibili
    {
      "Bilibili",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?bilibili\\.com",
      {"callback", "spm_id_from", "from_source", "from", "seid"},
      {"^https?:\\/\\/api\\.bilibili\\.com"}
    },
    // GOG
    {
      "GoodOldGames",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?gog\\.com",
      {"track_click", "link_id"},
      base::nullopt
    },
    // Bestbuy
    {
      "BestBuy",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?bestbuy\\.com",
      {"irclickid", "irgwc", "loc", "acampID", "mpid", "intl"},
      base::nullopt
    },
    // TikTok
    {
      "TikTok",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?tiktok\\.com",
      {"u_code", "preview_pb", "_d", "timestamp", "user_id",
      "share_app_name", "share_iid", "source"},
      base::nullopt
    },
    // Humble Bundle
    {
      "Humble Bundle",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?humblebundle\\.com",
      {"partner"},
      base::nullopt
    },
    // Epic Games
    {
      "Epic Games",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?epicgames\\.com",
      {"epic_affiliate", "epic_gameId"},
      base::nullopt
    },
    // Medium
    {
      "Medium",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?medium\\.com",
      {"source"},
      base::nullopt
    },
    // Sina Weibo
    {
      "Sina Weibo",
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?weibo\\.(com|cn)",
      {"sourceType", "from", "wm"},
      base::nullopt
    },
    // Weixin
    {
      "WeChat",
      "^https?:\\/\\/mp.weixin.qq\\.com",
      {"mpshare", "chksm"},
      base::nullopt
    },
  };

  return rules;
}
