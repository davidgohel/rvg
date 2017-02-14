#' Run plotting code and view svg in RStudio Viewer or web broswer.
#'
#' This is useful primarily for testing. Requires the \code{htmltools}
#' package.
#'
#' @param code Plotting code to execute.
#' @param ... Other arguments passed on to \code{\link{dsvg}}.
#' @export
#' @examples
#' \donttest{
#' if (require("htmltools")) {
#'   dsvg_view(plot(1:10))
#'   dsvg_view(hist(rnorm(100)))
#' }
#' }
#' @importFrom xml2 read_xml
dsvg_view <- function(code, ...) {
  path <- tempfile()
  dsvg(path, ...)
  tryCatch(code,
           finally = dev.off()
  )
  if( interactive() ){
    doc <- read_xml(path)
    htmltools::browsable(htmltools::HTML(as.character(doc)) )
  }
  else invisible()
}

