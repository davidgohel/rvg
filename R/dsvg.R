#' @title SVG Graphics Driver
#'
#' @description This function produces SVG files (compliant to the current w3 svg XML standard)
#' where elements can be made interactive.
#'
#' @details Graphic elements are made interactive with \code{jQuery} code. To embed
#' an interactive svg file into a web page, check that \code{jQuery} javascript file
#' is included.
#'
#' @param file the file where output will appear.
#' @param height,width Height and width in inches.
#' @param bg Default background color for the plot (defaults to "white").
#' @param pointsize default point size.
#' @param standalone Produce a stand alone svg file? If \code{FALSE}, omits
#'   xml header and default namespace.
#' @param canvas_id svg id within HTML page.
#' @seealso \code{\link{Devices}}
#' @examples
#' dsvg()
#' plot(rnorm(10), main="Simple Example", xlab = "", ylab = "")
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
  path <- tempfile()
  dsvg(path, ...)
  tryCatch(code,
           finally = dev.off()
  )
  htmltools::browsable(
    htmltools::HTML(paste0(readLines(path), collapse = "\n"))
  )
}

