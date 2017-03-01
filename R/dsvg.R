#' @title SVG Graphics Driver
#'
#' @description This function produces SVG files (compliant to the current w3 svg XML standard)
#' where elements can be made interactive.
#'
#' @param file the file where output will appear.
#' @param height,width Height and width in inches.
#' @param bg Default background color for the plot (defaults to "white").
#' @param pointsize default point size.
#' @param standalone Produce a stand alone svg file? If \code{FALSE}, omits
#'   xml header and default namespace.
#' @param canvas_id svg id within HTML page.
#' @param fontname_serif,fontname_sans,fontname_mono,fontname_symbol font
#' names for font faces. Used fonts should be available in the operating system.
#' These arguments are deprecated in favor of the \code{fonts} argument.
#' @param fonts Named list of font names to be aliased with
#'   fonts installed on your system. If unspecified, the R default
#'   families \code{sans}, \code{serif}, \code{mono} and \code{symbol}
#'   are aliased to the family returned by \code{\link[gdtools]{match_family}()}.
#' @seealso \code{\link{Devices}}
#' @examples
#' \donttest{
#' dsvg()
#' plot(rnorm(10), main="Simple Example", xlab = "", ylab = "")
#' dev.off()
#' }
#' @keywords device
#' @useDynLib rvg,.registration = TRUE
#' @importFrom Rcpp sourceCpp
#' @importFrom gdtools raster_view
#' @export
dsvg <- function(file = "Rplots.svg", width = 6, height = 6, bg = "white",
                pointsize = 12, standalone = TRUE, canvas_id = 1,
                fonts = list(),
                fontname_serif = NULL,
                fontname_sans = NULL,
                fontname_mono = NULL,
                fontname_symbol = NULL) {

  if( !is.null(fontname_serif) || !is.null(fontname_sans) ||
      !is.null(fontname_mono) || !is.null(fontname_symbol) ){
    warning("arguments fontname_(sans|serif|mono|symbol) are deprecated in favor of the fonts argument")
    fonts <- list()

    if( !is.null(fontname_serif) )
      fonts$serif <- fontname_serif
    if( !is.null(fontname_sans) )
      fonts$sans <- fontname_sans
    if( !is.null(fontname_mono) )
      fonts$mono <- fontname_mono
    if( !is.null(fontname_symbol) )
      fonts$symbol <- fontname_symbol
  }


  system_fonts <- validate_fonts( fonts )

  invisible(DSVG_(file=file, width=width, height=height, bg=bg,
                  pointsize=pointsize,
                  standalone=standalone,
                  canvas_id=canvas_id,
                  aliases = list(system = system_fonts, user = list())
                  )
            )
}

