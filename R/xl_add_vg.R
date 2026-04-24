#' @export
#' @title add a plot output as vector graphics into an Excel object
#' @description
#' Deprecated, use [sheet_add_drawing()] with a [dml()] object instead.
#' @param x an \code{rxlsx} object produced by \code{officer::read_xlsx}
#' @param code plot instructions
#' @param sheet sheet label/name
#' @param height,width Height and width in inches.
#' @param left,top left and top origin of the plot on the sheet in inches.
#' @param ... arguments passed on to \code{\link{dml_xlsx}}.
#' @examples
#' \donttest{
#' library(officer)
#' my_ws <- read_xlsx()
#' my_ws <- sheet_add_drawing(my_ws,
#'   value = dml(code = barplot(1:5, col = 2:6)),
#'   sheet = "Feuil1",
#'   width = 6, height = 6, left = 1, top = 2
#' )
#' fileout <- tempfile(fileext = ".xlsx")
#' print(my_ws, target = fileout)
#' }
xl_add_vg <- function(x, sheet, code, left, top, width, height, ...) {
  .Deprecated("sheet_add_drawing")
  sheet_add_drawing(
    x,
    value = dml(code = code, ...),
    sheet = sheet,
    left = left,
    top = top,
    width = width,
    height = height
  )
}
