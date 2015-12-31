#' @importFrom utils unzip
#' @import xml2
#' @title Microsoft Word Graphics Device
#'
#' @description
#' A graphical device for Microsoft Word documents.
#' @param file filename of the Microsoft Word document to produce. File
#' extension must be \code{.docx}.
#' @param code Plotting code to execute
#' @param pagesize Word document page size in inches.
#' A named vector (\code{width} and \code{height}).
#' @param margins Word document margins size in inches.
#' A named vector (\code{left}, \code{right}, \code{top}, \code{bottom}).
#' @param ... arguments for \code{fun} (passed on to \code{\link{dml_docx}}.)
#' @examples
#' write_docx(file = "my_plot_1.docx", code = plot(rnorm(10)) )
#' write_docx(file = "my_plot_2.docx", code = barplot(1:7, col = 1:7))
#' @keywords device
#' @export
write_docx <- function( file, code,
  pagesize = c(width = 8.5, height = 11),
  margins = c( left = 1, right = 1, top = 1, bottom = 1 ),
  ...) {

  .reg = regexpr( paste( "(\\.(?i)(docx))$", sep = "" ), file )
  if( .reg < 1 ) stop(file , " should have '.docx' as extension.")


  template_dir <- tempfile()
  unzip( zipfile = file.path( system.file(package = "rvg"), "templates/vanilla.docx" ), exdir = template_dir )
  drop_dir <- file.path(template_dir, "__MACOSX")
  if( dir.exists(drop_dir) ) unlink(drop_dir, force = TRUE, recursive = TRUE)

  document_rel <- file.path( template_dir, "word", "_rels/document.xml.rels" )
  relationships <- read_relationship( filename = document_rel )

  uid <- basename(tempfile(pattern = ""))
  img_directory = file.path(getwd(), uid )

  dml_file <- tempfile()

  pars <- list(...)
  pars$file <- dml_file
  pars$id <- 1L
  pars$next_rels_id <- as.integer( relationships$max_int - 1 )
  pars$raster_prefix <- img_directory
  pars$standalone <- FALSE

  do.call("dml_docx", pars)

  tryCatch(code, finally = dev.off() )


  raster_files <- list.files(path = getwd(), pattern = paste0("^", uid, "(.*)\\.png$"), full.names = TRUE )

  if( length(raster_files ) > 0 ){
    raster_names <- gsub( pattern = "\\.png$", replacement = "", basename(raster_files) )

    ids <- seq_along(raster_files) + relationships$max_int
    expected_rels <- data.frame(
      id = paste0("rId", ids ),
      int_id = ids,
      type = rep("http://schemas.openxmlformats.org/officeDocument/2006/relationships/image",
                 length(ids)),
      target = file.path("media", basename(raster_files) ),
      stringsAsFactors = FALSE )
    new_rels <- rbind( relationships$data, expected_rels)

    Relationship <- paste0("<Relationship Id=\"", new_rels$id, "\" Type=\"", new_rels$type, "\" Target=\"", new_rels$target , "\"/>")
    Relationship <- paste0(Relationship, collapse = "\n")

    sink(file = document_rel )
    cat("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n")
    cat("<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">\n")
    cat(Relationship)
    cat("\n</Relationships>")
    sink( )

    dir.create(file.path(template_dir, "word", "media"))
    for(i in seq_len(nrow(expected_rels))){
      file.copy(from = raster_files[i], to = file.path(template_dir, "word", "media"))
    }
    unlink(raster_files, force = TRUE)
  }
  pagesize <- ( pagesize * 20 * 72 )
  margins <- ( margins * 20 * 72 )

  document_xml <- file.path( template_dir, "word", "document.xml" )
  dml_str <- scan( dml_file, what = "character", quiet = T, sep = "\n" )

  sink(file = document_xml )
  cat("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>")
  cat("<w:document
  xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\"
  xmlns:w15=\"http://schemas.microsoft.com/office/word/2012/wordml\"
  xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\"
  xmlns:m=\"http://schemas.openxmlformats.org/officeDocument/2006/math\"
  xmlns:w14=\"http://schemas.microsoft.com/office/word/2010/wordml\"
  xmlns:wp=\"http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing\"
  xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\"
  xmlns:mc=\"http://schemas.openxmlformats.org/markup-compatibility/2006\"
  xmlns:ns9=\"http://schemas.openxmlformats.org/schemaLibrary/2006/main\"
  xmlns:wne=\"http://schemas.microsoft.com/office/word/2006/wordml\"
  xmlns:c=\"http://schemas.openxmlformats.org/drawingml/2006/chart\"
  xmlns:ns12=\"http://schemas.openxmlformats.org/drawingml/2006/chartDrawing\"
  xmlns:dgm=\"http://schemas.openxmlformats.org/drawingml/2006/diagram\"
  xmlns:pic=\"http://schemas.openxmlformats.org/drawingml/2006/picture\"
  xmlns:xdr=\"http://schemas.openxmlformats.org/drawingml/2006/spreadsheetDrawing\"
  xmlns:dsp=\"http://schemas.microsoft.com/office/drawing/2008/diagram\"
  xmlns:v=\"urn:schemas-microsoft-com:vml\" xmlns:o=\"urn:schemas-microsoft-com:office:office\"
  xmlns:ns19=\"urn:schemas-microsoft-com:office:excel\" xmlns:w10=\"urn:schemas-microsoft-com:office:word\"
  xmlns:ns21=\"urn:schemas-microsoft-com:office:powerpoint\" xmlns:ns23=\"http://schemas.microsoft.com/office/2006/coverPageProps\"
  xmlns:odx=\"http://opendope.org/xpaths\" xmlns:odc=\"http://opendope.org/conditions\"
  xmlns:odq=\"http://opendope.org/questions\" xmlns:oda=\"http://opendope.org/answers\"
  xmlns:odi=\"http://opendope.org/components\" xmlns:odgm=\"http://opendope.org/SmartArt/DataHierarchy\"
  xmlns:ns30=\"http://schemas.openxmlformats.org/officeDocument/2006/bibliography\"
  xmlns:ns31=\"http://schemas.openxmlformats.org/drawingml/2006/compatibility\"
  xmlns:ns32=\"http://schemas.openxmlformats.org/drawingml/2006/lockedCanvas\"
  xmlns:wpc=\"http://schemas.microsoft.com/office/word/2010/wordprocessingCanvas\"
  xmlns:wpg=\"http://schemas.microsoft.com/office/word/2010/wordprocessingGroup\"
  xmlns:wps=\"http://schemas.microsoft.com/office/word/2010/wordprocessingShape\">")

  cat("<w:body>")
  cat("<w:p>")
  cat("<w:bookmarkStart w:id=\"0\" w:name=\"_GoBack\"/><w:bookmarkEnd w:id=\"0\"/>")
  cat(dml_str)
  cat("</w:p>")
  cat("<w:sectPr>")
  cat("<w:pgSz w:w=\"")
  cat(as.integer(pagesize["width"]))
  cat("\" w:h=\"")
  cat(as.integer(pagesize["height"]))
  cat("\" w:code=\"1\" />")
  cat("<w:pgMar w:top=\"")
  cat(as.integer(margins["top"]))
  cat("\" w:right=\"")
  cat(as.integer(margins["right"]))
  cat("\" w:bottom=\"")
  cat(as.integer(margins["bottom"]))
  cat("\" w:left=\"")
  cat(as.integer(margins["left"]))
  cat("\"/>
  </w:sectPr>
  </w:body>
  </w:document>")
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







