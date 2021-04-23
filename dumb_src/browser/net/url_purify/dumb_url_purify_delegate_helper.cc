/* Copyright (c) 2020 The Dumb Browser Authors. All rights reserved.
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

#include "dumb/browser/net/url_purify/dumb_url_purify_delegate_helper.h"

#include <iostream>

#include "base/no_destructor.h"
#include "base/strings/string_util.h"
#include "base/synchronization/lock.h"
#include "content/public/common/referrer.h"
#include "extensions/common/url_pattern.h"
#include "net/url_request/url_request.h"

namespace {

struct MatcherRule {
  std::string url_pattern;
  std::vector<std::string> query_patterns;
  base::Optional<std::vector<std::string>> url_exceptions;

  MatcherRule(std::string url_pattern,
              std::vector<std::string> query_patterns,
              base::Optional<std::vector<std::string>> url_exceptions):
      url_pattern(std::move(url_pattern)),
      query_patterns(std::move(query_patterns)),
      url_exceptions(std::move(url_exceptions)) {}

  MatcherRule(std::string url_pattern,
              std::vector<std::string> query_patterns,
              std::vector<std::string> url_exceptions):
      url_pattern(std::move(url_pattern)),
      query_patterns(std::move(query_patterns)),
      url_exceptions(std::move(url_exceptions)) {}
};

struct SubQueryMatcher {
  std::unique_ptr<re2::RE2> url_matcher;
  base::Optional<re2::RE2*> url_exception_matcher;
  std::vector<std::unique_ptr<re2::RE2>> query_matchers;

  SubQueryMatcher(re2::RE2* url_matcher,
                  base::Optional<re2::RE2*> url_exception_matcher,
                  std::vector<std::unique_ptr<re2::RE2>> query_matchers):
    url_matcher(url_matcher),
    url_exception_matcher(std::move(url_exception_matcher)),
    query_matchers(std::move(query_matchers)) {}
};

// TODO: create a component to dynamically update rules

MatcherRule GetGlobalRules() {
  static MatcherRule rule = MatcherRule {
    ".*",
    {"utm(?:_[a-z_]*)?", "ga_[a-z_]+", "yclid", "_openstat",
     "fb_action_(?:types|ids)", "fb_(?:source|ref)", "fbclid",
     "action_(?:object|type|ref)_map", "gs_l", "mkt_tok",
     "hmb_(?:campaign|medium|source)", "ref_?", "referrer",
     "gclid", "otm_[a-z_]*", "cmpid", "os_ehash", "_ga",
     "__twitter_impression", "wt_?z?mc", "wtrid", "[a-z]?mc",
     "dclid", "Echobox", "spm", "vn(?:_[a-z]*)+", "tracking_source"},
    base::nullopt
  };

  return rule;
}

std::vector<MatcherRule> GetPerSiteRules() {
  return std::vector<MatcherRule> {
    // Google
    {
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
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?google(?:\\.[a-z]{2,}){1,}\\/search\\?",
      {"client", "sclient"},
      base::nullopt
    },
    // Amazon
    {
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
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?youtube\\.com",
      {"feature", "gclid", "kw"},
      base::nullopt
    },
    // Facebook
    {
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
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?instagram\\.com",
      {"igshid"},
      base::nullopt
    },
    // Steampowered
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?steampowered\\.com",
      {"snr"},
      base::nullopt
    },
    // GitHub
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?github\\.com",
      {"email_token", "email_source"},
      base::nullopt
    },
    // LinkedIn
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?linkedin\\.com",
      {"refId", "trk", "li[a-z]{2}"},
      base::nullopt
    },
    // lazada.com.my
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?lazada\\.com\\.my",
      {"ad_src", "did", "pa", "mp", "impsrc", "cid", "pos"},
      base::nullopt
    },
    // disq.us
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?disq\\.us",
      {"cuid"},
      base::nullopt
    },
    // Mozilla
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?mozilla\\.org",
      {"src", "platform", "redirect_source"},
      {"^https?:\\/\\/(?:[a-z0-9-]+\\.)*?mozilla.org\\/api"}
    },
    // New York Times
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?nytimes\\.com",
      {"smid"},
      base::nullopt
    },
    // Twitter
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?twitter.com",
      {"(?:ref_?)?src", "s", "cn", "ref_url"},
      base::nullopt
    },
    // Reddit
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?reddit.com",
      {"%24deep_link", "\\$deep_link", "correlation_id", "ref_campaign",
       "ref_source", "%243p", "\\$3p", "%24original_url", "\\$original_url",
       "_branch_match_id"},
      base::nullopt
    },
    // Netflix
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?netflix.com",
      {"trackId", "tctx", "jb[a-z]*?"},
      base::nullopt
    },
    // TechCrunch
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?techcrunch\\.com",
      {"ncid", "sr", "sr_share"},
      base::nullopt
    },
    // Bing
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?bing(?:\\.[a-z]{2,}){1,}",
      {"cvid", "form", "sk", "sp", "sc", "qs", "qp"},
      {"^https?:\\/\\/(?:[a-z0-9-]+\\.)*?bing(?:\\.[a-z]{2,}){1,}\\/WS\\/redirect\\/"}
    },
    // Twitch
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?twitch\\.com",
      {"tt_medium", "tt_content"},
      base::nullopt
    },
    // IMDB
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?imdb\\.com",
      {"ref_", "pf_rd_[a-z]*"},
      base::nullopt
    },
    // Wallmart
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?walmart\\.com",
      {"u1", "ath[a-z]*"},
      base::nullopt
    },
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?youku\\.com",
      {"tpa"},
      base::nullopt
    },
    // Taobao
    {
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
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?tb\\.cn",
      {"sm"},
      base::nullopt
    },
    // Aliexpress
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?aliexpress(?:\\.[a-z]{2,}){1,}",
      {"ws_ab_test", "btsid", "algo_expid", "algo_pvid", "gps-id",
       "scm[_a-z-]*", "cv", "af", "mall_affr", "sk", "dp", "terminal_id",
       "aff_request_id"},
      base::nullopt
    },
    // Bilibili
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?bilibili\\.com",
      {"callback", "spm_id_from", "from_source", "from", "seid"},
      {"^https?:\\/\\/api\\.bilibili\\.com"}
    },
    // GOG
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?gog\\.com",
      {"track_click", "link_id"},
      base::nullopt
    },
    // Bestbuy
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?bestbuy\\.com",
      {"irclickid", "irgwc", "loc", "acampID", "mpid", "intl"},
      base::nullopt
    },
    // TikTok
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?tiktok\\.com",
      {"u_code", "preview_pb", "_d", "timestamp", "user_id",
      "share_app_name", "share_iid", "source"},
      base::nullopt
    },
    // Humble Bundle
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?humblebundle\\.com",
      {"partner"},
      base::nullopt
    },
    // Epic Games
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?epicgames\\.com",
      {"epic_affiliate", "epic_gameId"},
      base::nullopt
    },
    // Medium
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?medium\\.com",
      {"source"},
      base::nullopt
    },
    // Sina Weibo
    {
      "^https?:\\/\\/(?:[a-z0-9-]+\\.)*?weibo\\.(com|cn)",
      {"sourceType", "from", "wm"},
      base::nullopt
    },
    // Weixin
    {
      "^https?:\\/\\/mp.weixin.qq\\.com",
      {"mpshare", "chksm"},
      base::nullopt
    },
  };
}

SubQueryMatcher BuildMatcher(const std::string& url_pattern,
                             const std::vector<std::string>& query_patterns,
                             base::Optional<std::vector<std::string>>
                             url_exceptions = base::nullopt) {
  re2::RE2::Options options;
  options.set_case_sensitive(false);
  // build url_matcher
  auto* url_matcher = new re2::RE2(url_pattern, options);
  DCHECK(url_matcher->ok());
  // build exceptions matcher
  base::Optional<re2::RE2*> url_exception_matcher;
  if(url_exceptions) {
    const std::string exception_pattern(
      base::JoinString(url_exceptions.value(), "|"));
    url_exception_matcher = new re2::RE2(exception_pattern, options);
    DCHECK(url_exception_matcher.value()->ok());
  }
  // build query matcher
  std::vector<std::unique_ptr<re2::RE2>> query_matchers;
  const std::string query_pattern(base::JoinString(query_patterns, "|"));

  // appended matcher
  auto* query_appended_matcher = new re2::RE2(
    "^(" + query_pattern + ")=[^&]+$", options);
  DCHECK(query_appended_matcher->ok());
  query_matchers.emplace_back(query_appended_matcher);

  // first matcher
  auto* query_first_matcher = new re2::RE2(
    "^(" + query_pattern + ")=[^&]+&", options);
  DCHECK(query_first_matcher->ok());
  query_matchers.emplace_back(query_first_matcher);

  // only matcher
  auto* query_only_matcher = new re2::RE2(
    "&(" + query_pattern + ")=[^&]+", options);
  DCHECK(query_only_matcher->ok());
  query_matchers.emplace_back(query_only_matcher);

  return SubQueryMatcher(url_matcher,
                         url_exception_matcher,
                         std::move(query_matchers));
}

#define RE2UNANCHORED re2::RE2::Anchor::UNANCHORED

class QueryMatcher {
public:
  QueryMatcher():
      global_matcher_(BuildMatcher(GetGlobalRules().url_pattern,
                                   GetGlobalRules().query_patterns,
                                   GetGlobalRules().url_exceptions)) {

    // Build other matchers
    auto per_site_rules = GetPerSiteRules();
    for(const auto& rule: per_site_rules) {
      matchers_.emplace_back(BuildMatcher(
          rule.url_pattern, rule.query_patterns, rule.url_exceptions));
    }
  }

  int FilterQuery(const std::string& full_url, std::string& new_query) {
    int count = 0;

    // Apply global rules.
    count += TryApplyMatcher(global_matcher_, full_url, new_query).value();

    for(const auto& matcher: matchers_) {
      auto result = TryApplyMatcher(matcher, full_url, new_query);
      if(result.has_value()) {
        if(result.value() == -1) {
          // Not match
          break;
        }
        count += result.value();
        break;
      }
    }

    return count;
  }

private:
  base::Optional<int> TryApplyMatcher(const SubQueryMatcher& matcher,
                                      const std::string& full_url,
                                      std::string& new_query) {
    int count = 0;
    const auto url_len = full_url.length() - 1;

    // Skip if match any exception.
    if(matcher.url_exception_matcher.has_value() &&
        matcher.url_exception_matcher.value()->Match(
            full_url, 0, url_len, RE2UNANCHORED, nullptr, 0)) {
      return -1;
    }

    // filter
    for(const auto& query_matcher: matcher.query_matchers) {
      count += re2::RE2::GlobalReplace(&new_query, *query_matcher.get(), "");
    }

    return count;
  }

  SubQueryMatcher global_matcher_;
  std::vector<SubQueryMatcher> matchers_;
  base::Lock lock_;
};

QueryMatcher& GetMatcher() {
  static base::NoDestructor<QueryMatcher> instance;
  return *instance;
}

void ApplyRules(std::shared_ptr<dumb::DumbRequestInfo>& ctx) {
  if (ctx->redirect_source.is_valid()) {
    if (ctx->internal_redirect) {
      // Ignore internal redirects since we trigger them.
      return;
    }

    if (net::registry_controlled_domains::SameDomainOrHost(
            ctx->redirect_source, ctx->request_url,
            net::registry_controlled_domains::INCLUDE_PRIVATE_REGISTRIES)) {
      // Same-site redirects are exempted.
      return;
    }
  } else if (ctx->initiator_url.is_valid() &&
             net::registry_controlled_domains::SameDomainOrHost(
                 ctx->initiator_url, ctx->request_url,
                 net::registry_controlled_domains::
                     INCLUDE_PRIVATE_REGISTRIES)) {
    // Same-site requests are exempted.
    return;
  }

  std::string new_query = ctx->request_url.query();
  const std::string& full_url = ctx->request_url.spec();
  // std::cout << "Full URL: " << full_url << std::endl;

  int replacement_count = GetMatcher().FilterQuery(full_url, new_query);

  if (replacement_count > 0) {
    url::Replacements<char> replacements;
    if (new_query.empty()) {
      replacements.ClearQuery();
    } else {
      replacements.SetQuery(new_query.c_str(),
                            url::Component(0, new_query.size()));
    }
    ctx->new_url_spec = ctx->request_url.ReplaceComponents(replacements).spec();
    // std::cout << "Filtered URL: " << *new_url_spec << std::endl;
  }
}

} // namespace

namespace dumb {

int OnBeforeURLRequest_URLPurifyWork(const ResponseCallback& next_callback,
                                     std::shared_ptr<DumbRequestInfo> ctx) {
  if (ctx->request_url.has_query()) {
    ApplyRules(ctx);
  }

  return net::OK;
}

int OnBeforeStartTransaction_URLPurifyWork(
    net::HttpRequestHeaders* headers,
    const ResponseCallback& next_callback,
    std::shared_ptr<DumbRequestInfo> ctx) {
  return net::OK;
}

}  // namespace dumb
