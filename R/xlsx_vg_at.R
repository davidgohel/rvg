#' @export
#' @title add a plot output as vector graphics into an Excel object
#' @description produces a vector graphics output from R plot instructions
#' and add the result in an Excel sheet.
#' by \code{\link[officer]{read_xlsx}}.
#' @param x an \code{rxlsx} object produced by \code{officer::read_xlsx}
#' @param code plot instructions
#' @param sheet_id sheet id
#' @param height,width Height and width in inches.
#' @param left,top left and top origin of the plot on the slide in inches.
#' @param ... arguments passed on to \code{\link{dml_xlsx}}.
#' @importFrom xml2 xml_ns xml_new_root xml_attr<- xml_add_sibling
xlsx_vg <- function( x, sheet_id, code, left, top, width, height, ... ){

  stopifnot(inherits(x, "rxlsx"))

  img_directory = get_img_dir()

  dml_file <- file.path( x$package_dir, "xl/drawings",
                         basename( tempfile(pattern = "d", fileext = ".xml") ) )
  dir.create(dirname(dml_file), showWarnings = FALSE)
  sheet <- x$sheets$get_sheet(sheet_id)

  pars <- list(...)

  pars$file <- dml_file
  pars$id <- 0L
  pars$last_rel_id <- sheet$relationship()$get_next_id() - 1
  pars$raster_prefix <- img_directory
  pars$standalone <- TRUE
  pars$width <- width
  pars$height <- height
  pars$offx <- left
  pars$offy <- top

  do.call("dml_xlsx", pars)

  tryCatch(code, finally = dev.off() )
  raster_files <- list_raster_files(img_dir = img_directory )

  if( length(raster_files) ){
    browser()
    # slide$reference_img(src = raster_files, dir_name = file.path(x$package_dir, "ppt/media"))
    unlink(raster_files, force = TRUE)
  }
  rel <- sheet$relationship()

  dml_rid <- rel$get_next_id()
  rel$add_drawing(src = dml_file, root_target = "../drawings")

  sheet_ <- x$sheets$get_sheet(sheet_id)
  extLst <- xml_find_first(sheet_$get(), "d1:extLst", xml_ns(sheet_$get()) )

  xml_elt <- xml_new_root("drawing")
  xml_attr(xml_elt, "r:id") <- sprintf("rId%.0f", dml_rid)
  xml_add_sibling(.x = extLst, .value = xml_elt, .where = "before")


  override <- c("application/vnd.openxmlformats-officedocument.drawing+xml" )
  names(override) <- paste0("/xl/drawings/", basename(dml_file))
  x$content_type$add_override(value = override)
  sheet_$save()


  x
}

