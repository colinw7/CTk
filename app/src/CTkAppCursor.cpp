#include <CTkAppCursor.h>

struct NameCursor {
  QString         name;
  Qt::CursorShape shape { Qt::ArrowCursor };
};

/*
Qt::SizeAllCursor
Qt::BlankCursor
Qt::SplitVCursor
Qt::SplitHCursor
Qt::PointingHandCursor
Qt::ForbiddenCursor
Qt::BusyCursor
*/

static NameCursor nameCursors[] = {
  { "X_cursor"           , Qt::ArrowCursor },
  { "arrow"              , Qt::ArrowCursor },
  { "based_arrow_down"   , Qt::ArrowCursor },
  { "based_arrow_up"     , Qt::UpArrowCursor },
  { "boat"               , Qt::ArrowCursor },
  { "bogosity"           , Qt::ArrowCursor },
  { "bottom_left_corner" , Qt::ArrowCursor },
  { "bottom_right_corner", Qt::SizeFDiagCursor },
  { "bottom_side"        , Qt::ArrowCursor },
  { "bottom_tee"         , Qt::ArrowCursor },
  { "box_spiral"         , Qt::ArrowCursor },
  { "center_ptr"         , Qt::ArrowCursor },
  { "circle"             , Qt::ArrowCursor },
  { "clock"              , Qt::ArrowCursor },
  { "coffee_mug"         , Qt::ArrowCursor },
  { "cross"              , Qt::CrossCursor },
  { "cross_reverse"      , Qt::ArrowCursor },
  { "crosshair"          , Qt::ArrowCursor },
  { "diamond_cross"      , Qt::ArrowCursor },
  { "dot"                , Qt::ArrowCursor },
  { "dotbox"             , Qt::ArrowCursor },
  { "double_arrow"       , Qt::ArrowCursor },
  { "draft_large"        , Qt::ArrowCursor },
  { "draft_small"        , Qt::ArrowCursor },
  { "draped_box"         , Qt::ArrowCursor },
  { "exchange"           , Qt::ArrowCursor },
  { "fleur"              , Qt::ArrowCursor },
  { "gobbler"            , Qt::ArrowCursor },
  { "gumby"              , Qt::ArrowCursor },
  { "hand1"              , Qt::OpenHandCursor },
  { "hand2"              , Qt::ClosedHandCursor },
  { "heart"              , Qt::ArrowCursor },
  { "icon"               , Qt::ArrowCursor },
  { "iron_cross"         , Qt::ArrowCursor },
  { "left_ptr"           , Qt::ArrowCursor },
  { "left_side"          , Qt::ArrowCursor },
  { "left_tee"           , Qt::ArrowCursor },
  { "leftbutton"         , Qt::ArrowCursor },
  { "ll_angle"           , Qt::ArrowCursor },
  { "lr_angle"           , Qt::ArrowCursor },
  { "man"                , Qt::ArrowCursor },
  { "middlebutton"       , Qt::ArrowCursor },
  { "mouse"              , Qt::ArrowCursor },
  { "pencil"             , Qt::ArrowCursor },
  { "pirate"             , Qt::ArrowCursor },
  { "plus"               , Qt::ArrowCursor },
  { "question_arrow"     , Qt::WhatsThisCursor },
  { "right_ptr"          , Qt::ArrowCursor },
  { "right_side"         , Qt::ArrowCursor },
  { "right_tee"          , Qt::ArrowCursor },
  { "rightbutton"        , Qt::ArrowCursor },
  { "rtl_logo"           , Qt::ArrowCursor },
  { "sailboat"           , Qt::ArrowCursor },
  { "sb_down_arrow"      , Qt::ArrowCursor },
  { "sb_h_double_arrow"  , Qt::SizeHorCursor },
  { "sb_left_arrow"      , Qt::ArrowCursor },
  { "sb_right_arrow"     , Qt::ArrowCursor },
  { "sb_up_arrow"        , Qt::ArrowCursor },
  { "sb_v_double_arrow"  , Qt::SizeVerCursor },
  { "shuttle"            , Qt::ArrowCursor },
  { "sizing"             , Qt::ArrowCursor },
  { "spider"             , Qt::ArrowCursor },
  { "spraycan"           , Qt::ArrowCursor },
  { "star"               , Qt::ArrowCursor },
  { "target"             , Qt::ArrowCursor },
  { "tcross"             , Qt::ArrowCursor },
  { "top_left_arrow"     , Qt::SizeBDiagCursor },
  { "top_left_corner"    , Qt::ArrowCursor },
  { "top_right_corner"   , Qt::ArrowCursor },
  { "top_side"           , Qt::ArrowCursor },
  { "top_tee"            , Qt::ArrowCursor },
  { "trek"               , Qt::ArrowCursor },
  { "ul_angle"           , Qt::ArrowCursor },
  { "umbrella"           , Qt::ArrowCursor },
  { "ur_angle"           , Qt::ArrowCursor },
  { "watch"              , Qt::WaitCursor },
  { "xterm"              , Qt::IBeamCursor },
  { }
};

//---

CTkAppCursor *
CTkAppCursorMgr::
getCursor(const QString &name)
{
  auto pc = cursors_.find(name);
  if (pc != cursors_.end()) return (*pc).second;

  if (name == "none")
    return getCursor("arrow");

  for (int i = 0; nameCursors[i].name != ""; ++i) {
    if (nameCursors[i].name == name) {
      auto *cursor = new CTkAppCursor(tk_, name);

      cursors_[name] = cursor;

      return cursor;
    }
  }

  return nullptr;
}

//---

CTkAppCursor::
CTkAppCursor(CTkApp *tk, const QString &name) :
 tk_(tk), name_(name)
{
}

CTkAppCursor::
~CTkAppCursor()
{
}
