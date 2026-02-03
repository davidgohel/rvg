#' @export
#' @keywords internal
#' @title Defunct Functions in Package rvg
#' @param ... unused argument
#' @details `body_add_vg()` is removed and can be replaced by `officer::body_add_img`.
body_add_vg <- function(...) {
  .Defunct("officer::body_add_img")
}
