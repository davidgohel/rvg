#' @importFrom utils unzip
#' @import xml2
#' @title Microsoft PowerPoint Graphics Device
#'
#' @description
#' A graphical device for Microsoft PowerPoint documents.
#' @param file filename of the Microsoft PowerPoint document to produce. File
#' extension must be \code{.pptx}.
#' @param code Plotting code to execute
#' @param size slide size in inches.
#' @param ... arguments for \code{fun} (passed on to \code{\link{dml_pptx}}.)
#' @examples
#' write_pptx(file = "my_plot_1.pptx", code = plot(rnorm(10)))
#' write_pptx(file = "my_plot_2.pptx", code = barplot(1:7, col = 1:7))
#' @keywords device
#' @export
write_pptx <- function(
  file, code,
  size = c(width = 10, height = 7.5), ...) {

  .reg = regexpr( paste( "(\\.(?i)(pptx))$", sep = "" ), file )
  if( .reg < 1 ) stop(file , " should have '.pptx' as extension.")

  template_dir <- tempfile()
  unzip( zipfile = file.path( system.file(package = "rvg"), "templates/vanilla.pptx" ), exdir = template_dir )
  drop_dir <- file.path(template_dir, "__MACOSX")
  if( dir.exists(drop_dir) ) unlink(drop_dir, force = TRUE, recursive = TRUE)

  document_rel <- file.path( template_dir, "ppt/slides/", "_rels/slide1.xml.rels" )
  relationships <- read_relationship( filename = document_rel )

  uid <- basename(tempfile(pattern = ""))
  img_directory = file.path(getwd(), uid )

  dml_file <- tempfile()

  pars <- list(...)
  pars$file <- dml_file
  pars$id <- 0L
  pars$next_rels_id <- as.integer( relationships$max_int - 1 )
  pars$raster_prefix <- img_directory
  pars$standalone <- FALSE

  do.call("dml_pptx", pars)

  tryCatch(code, finally = dev.off() )

  raster_files <- list.files(path = getwd(), pattern = paste0("^", uid, "(.*)\\.png$"), full.names = TRUE )

  if( length(raster_files ) > 0 ){
    raster_names <- gsub( pattern = "\\.png$", replacement = "", basename(raster_files) )


    ids <- seq_along(raster_files) + relationships$max_int
    expected_rels <- data.frame(
      id = paste0("rId", ids ),
      int_id = ids,
      type = rep("http://schemas.openxmlformats.org/officeDocument/2006/relationships/image", length(ids)),
      target = file.path("../media", basename(raster_files) ),
      stringsAsFactors = FALSE )
    new_rels <- rbind( relationships$data, expected_rels)

    Relationship <- paste0("<Relationship Id=\"", new_rels$id, "\" Type=\"", new_rels$type, "\" Target=\"", new_rels$target , "\"/>")
    Relationship <- paste0(Relationship, collapse = "")

    sink(file = document_rel )
    cat("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>")
    cat("<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">")
    cat(Relationship)
    cat("</Relationships>")
    sink( )

    document_rel <- file.path( template_dir, "ppt/slides/", "_rels/slide1.xml.rels" )

    media_dir <- file.path(template_dir, "ppt", "media")
    if( !dir.exists(media_dir))
      dir.create(media_dir)

    for(i in seq_len(nrow(expected_rels))){
      file.copy(from = raster_files[i], to = file.path(template_dir, "ppt", "media"))
    }
    unlink(raster_files, force = TRUE)
  }

  document_xml <- file.path( template_dir, "ppt/slides/", "slide1.xml" )
  dml_str <- scan( dml_file, what = "character", quiet = T, sep = "\n" )
  sink(file = document_xml )
  cat("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n")
  cat("<p:sld ")
  cat("xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" ")
  cat("xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" ")
  cat("xmlns:p=\"http://schemas.openxmlformats.org/presentationml/2006/main\" ")
  cat("xmlns:pic=\"http://schemas.openxmlformats.org/drawingml/2006/picture\"")
  cat(">")

  cat("<p:cSld>")
  cat(dml_str)
  cat("</p:cSld>")
  cat("</p:sld>")
  sink()

  # set slide size
  slide_size_str <- sprintf( "<p:sldSz cx=\"%d\" cy=\"%d\"/>", as.integer(size["width"] * 914400),  as.integer(size["height"] * 914400) )
  presentation_file <- file.path( template_dir, "ppt", "presentation.xml" )
  presentation_str <- scan( presentation_file, what = "character", quiet = T, sep = "\n" )
  presentation_str <- gsub(pattern = "<p:sldSz cx=\"9144000\" cy=\"6858000\" type=\"screen4x3\"/>",
       replacement =  slide_size_str, x = presentation_str )
  sink(file = presentation_file )
  cat(presentation_str, sep = "")
  sink()


  # delete out_file if existing
  if( file.exists(file))
    unlink(file, force = TRUE)
  # write out_file
  out_file <- pack_folder(template_dir, file )
  # delete temporary dir
  unlink(template_dir, recursive = TRUE, force = TRUE)
  out_file
}

