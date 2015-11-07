#' A SVG Graphics Driver
#'
#' This function produces graphics suitable the current w3 svg XML standard.
#' It currently does not have any font metric information, so the use of
#' \code{\link{plotmath}} is not supported. The driver output is currently NOT
#' specifying a DOCTYPE DTD
#'
#' @param file the file where output will appear.
#' @param height,width Height and width in inches.
#' @param bg Default background color for the plot (defaults to "white").
#' @param pointsize default point size.
#' @param standalone Produce a stand alone svg file? If \code{FALSE}, omits
#'   xml header and default namespace.
#' @param canvas_id svg id within HTML page.
#' @seealso \code{\link{pictex}}, \code{\link{postscript}}, \code{\link{Devices}}
#' @examples
#' dsvg()
#' plot(1:11,(-5:5)^2, type='b', main="Simple Example")
#' dev.off()
#' @keywords device
#' @useDynLib rvg
#' @importFrom Rcpp sourceCpp
#' @importFrom gdtools raster_view
#' @export
#' @export
dsvg <- function(file = "Rplots.svg", width = 10, height = 8, bg = "white",
                pointsize = 12, standalone = TRUE, canvas_id = 1 ) {

  invisible(devSVG_(file, bg, width, height, pointsize, standalone, canvas_id))
}

mini_plot <- function(...) plot(..., axes = FALSE, xlab = "", ylab = "")

inlineDSVG <- function(code, ..., path = tempfile()) {

  dsvg(path, ...)
  tryCatch(code,
           finally = dev.off()
  )

  path
}

#' Run plotting code and view svg in RStudio Viewer or web broswer.
#'
#' This is useful primarily for testing. Requires the \code{htmltools}
#' package.
#'
#' @param code Plotting code to execute.
#' @param ... Other arguments passed on to \code{\link{dsvg}}.
#' @export
#' @examples
#' if (require("htmltools")) {
#'   htmlDSVG(plot(1:10))
#'   htmlDSVG(hist(rnorm(100)))
#' }
htmlDSVG <- function(code, ...) {
  tmp <- inlineDSVG(code, ...)
  htmltools::browsable(
    htmltools::HTML(paste0(readLines(tmp), collapse = "\n"))
  )
}

#' Run plotting code and return svg
#'
#' This is useful primarily for testing. Requires the \code{xml2} package.
#'
#' @return A \code{xml2::xml_document} object.
#' @param code Plotting code to execute.
#' @param ... Other arguments passed on to \code{\link{dsvg}}.
#' @export
#' @examples
#' if (require("xml2")) {
#'   x <- xmlDSVG(plot(1, axes = FALSE))
#'   x
#'   xml_find_all(x, ".//text")
#' }
xmlDSVG <- function(code, ...) {
  plot <- inlineDSVG(code, ..., standalone = FALSE)
  xml2::read_xml(plot)
}



