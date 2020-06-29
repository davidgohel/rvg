#' @name rvg-defunct
#' @title Defunct Functions in Package rvg
NULL


#' @export
#' @rdname rvg-defunct
#' @param ... unused arguments
#' @details `dml_docx()` is removed and can be replaced by `devEMF::emf`.
dml_docx <- function(...){
  .Defunct("devEMF::emf")
}
#' @export
#' @rdname rvg-defunct
#' @details `body_add_vg()` is removed and can be replaced by `officer::body_add_img`.
body_add_vg <- function(...){
  .Defunct("officer::body_add_img")
}
