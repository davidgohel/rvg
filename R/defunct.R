#' @name rvg-defunct
#' @title Defunct Functions in Package rvg
NULL

#' @export
#' @rdname rvg-defunct
#' @details `ph_with_vg()` is replaced by `officer::ph_with`.
#' @param ... unused arguments
ph_with_vg <- function(...){
  .Defunct("officer::ph_with")
}

#' @export
#' @rdname rvg-defunct
#' @details `ph_with_vg_at()` is replaced by `officer::ph_with`.
ph_with_vg_at <- function(...){
  .Defunct("officer::ph_with")
}
