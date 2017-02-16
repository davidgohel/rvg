#' @importFrom officer pack_folder unpack_folder
#' @import xml2
#' @title Microsoft Excel Graphics Device
#'
#' @description
#' A graphical device for Microsoft Excel documents.
#' @param file filename of the Microsoft Excel document to produce. File
#' extension must be \code{.xlsx}.
#' @param code Plotting code to execute
#' @param ... arguments for \code{fun} (passed on to \code{\link{dml_xlsx}}.)
#' @examples
#' \donttest{
#' write_xlsx(file = "my_plot_1.xlsx", code = plot(rnorm(10)))
#' write_xlsx(file = "my_plot_2.xlsx", code = barplot(1:7, col = 1:7))
#' }
#' @keywords device
#' @export
write_xlsx <- function(
  file, code,
  ...) {

  if( !grepl(x = file, pattern = "\\.(xlsx)$", ignore.case = TRUE) )
    stop(file , " should have '.xlsx' as extension.")

  template_dir <- tempfile(tmpdir = ".")
  zipfile <- file.path( system.file(package = "rvg"), "templates/vanilla.xlsx" )
  unpack_folder(file = zipfile, folder = template_dir )

  document_rel_dir <- file.path( template_dir, "xl/drawings/_rels" )
  document_rel <- file.path( document_rel_dir, "drawing1.xml.rels" )
  if( !file.exists(document_rel_dir))
    dir.create(document_rel_dir, recursive = TRUE)

  uid <- basename(tempfile(pattern = ""))
  img_directory = file.path(getwd(), uid )
  dml_file <- file.path( template_dir, "xl/drawings/drawing1.xml" )

  pars <- list(...)
  pars$file <- dml_file
  pars$id <- 0L
  pars$next_rels_id <- 0L
  pars$raster_prefix <- img_directory
  pars$standalone <- TRUE

  do.call("dml_xlsx", pars)

  tryCatch(code, finally = dev.off() )

  raster_files <- list.files(path = getwd(), pattern = paste0("^", uid, "(.*)\\.png$"), full.names = TRUE )

  if( length(raster_files ) > 0 ){
    raster_names <- gsub( pattern = "\\.png$", replacement = "", basename(raster_files) )

    ids <- seq_along(raster_files)
    new_rels <- data.frame(
      id = paste0("rId", ids ),
      int_id = ids,
      type = rep("http://schemas.openxmlformats.org/officeDocument/2006/relationships/image", length(ids)),
      target = file.path("../media", basename(raster_files) ),
      stringsAsFactors = FALSE )

    Relationship <- paste0("<Relationship Id=\"", new_rels$id, "\" Type=\"", new_rels$type, "\" Target=\"", new_rels$target , "\"/>")
    Relationship <- paste0(Relationship, collapse = "")

    sink(file = document_rel )
    cat("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>")
    cat("<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">")
    cat(Relationship)
    cat("</Relationships>")
    sink( )

    media_dir <- file.path(template_dir, "xl", "media")
    if( !file.exists(media_dir))
      dir.create(media_dir)

    for(i in seq_len(nrow(new_rels))){
      file.copy(from = raster_files[i], to = media_dir)
    }
    unlink(raster_files, force = TRUE)
  }

  document_xml <- file.path( template_dir, "xl/worksheets/", "sheet1.xml" )
  sink(file = document_xml )
  cat("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n")
  cat("<worksheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" ")
  cat("xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\"")
  cat(">")
  cat("<dimension ref=\"A1\"/>")
  cat("<sheetViews><sheetView tabSelected=\"1\" workbookViewId=\"0\"/></sheetViews><sheetData/>")
  cat("<drawing r:id=\"rId1\"/>")
  cat("</worksheet>")
  sink()

  document_xml_rels_dir <- file.path( template_dir, "xl/worksheets/_rels" )
  document_xml_rels <- file.path( document_xml_rels_dir, "sheet1.xml.rels" )
  if( !file.exists(document_xml_rels_dir))
    dir.create(document_xml_rels_dir, recursive = TRUE)
  sink(file = document_xml_rels )
  cat("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n")
  cat("<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\"><Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/drawing\" Target=\"../drawings/drawing1.xml\"/></Relationships>")
  sink( )

  # delete out_file if existing
  if( file.exists(file))
    unlink(file, force = TRUE)
  # write out_file
  out_file <- pack_folder(template_dir, file )
  # delete temporary dir
  unlink(template_dir, recursive = TRUE, force = TRUE)
  out_file
}





