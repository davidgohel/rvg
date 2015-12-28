pack_folder <- function( folder, target ){
  target <- normalizePath(path.expand(target), mustWork = FALSE, winslash = "/")
  curr_wd <- getwd()
  zip_dir <- folder
  setwd(zip_dir)
  zip(zipfile = target, flags = "-r9Xq",
      files = list.files(all.files = TRUE, recursive = TRUE, include.dirs = FALSE))
  setwd(curr_wd)
  target
}

read_relationship <- function(filename) {
  doc <- read_xml( x = filename )
  children <- xml_children( doc )
  ns <- xml_ns( doc )
  id <- sapply( children, xml_attr, attr = "Id", ns)
  int_id <- as.integer( gsub(pattern = "^rId", replacement = "", x = id ) )
  type <- sapply( children, xml_attr, attr = "Type", ns)
  target <- sapply( children, xml_attr, attr = "Target", ns)
  out <- list( data = data.frame(id = id, int_id = int_id, type = type, target = target, stringsAsFactors = FALSE ) )
  out$max_int <- max(int_id, na.rm = T)
  out
}



#' @import xml2
#' @export
docx_wrapper <- function(
  file, fun,
  pagesize = c(width = 8.5, height = 11),
  margins = c( left = 1, right = 1, top = 1, bottom = 1 ),
  width = 6, height = 6,
  bg = "white",
  fontname_serif = getOption("rvg_fonts")$fontname_serif,
  fontname_sans = getOption("rvg_fonts")$fontname_sans,
  fontname_mono = getOption("rvg_fonts")$fontname_mono,
  fontname_symbol = getOption("rvg_fonts")$fontname_symbol,
  pointsize = 12, editable = TRUE,
  ...) {

  template_dir <- tempfile()
  unzip( zipfile = file.path( system.file(package = "rvg"), "templates/vanilla.docx" ), exdir = template_dir )
  drop_dir <- file.path(template_dir, "__MACOSX")
  if( dir.exists(drop_dir) ) unlink(drop_dir, force = TRUE, recursive = TRUE)

  document_rel <- file.path( template_dir, "word", "_rels/document.xml.rels" )
  relationships <- read_relationship( filename = document_rel )

  uid <- basename(tempfile(pattern = ""))
  img_directory = file.path(getwd(), uid )

  dml_file <- tempfile()
  dml_docx(file = dml_file, width = width, height = height, bg = bg,
           fontname_serif = fontname_serif, fontname_sans = fontname_sans,
           fontname_mono = fontname_mono, fontname_symbol = fontname_symbol,
           pointsize = pointsize, editable = editable,
           id = 1L,
           next_rels_id = as.integer( relationships$max_int - 1 ),
           raster_prefix = img_directory, standalone = FALSE
           )
  tryCatch(fun(...),
           finally = dev.off()
  )


  raster_files <- list.files(path = getwd(), pattern = paste0("^", uid, "(.*)\\.png$"), full.names = TRUE )

  if( length(raster_files ) > 0 ){
    raster_names <- gsub( pattern = "\\.png$", replacement = "", basename(raster_files) )


    ids <- seq_along(raster_files) + relationships$max_int
    expected_rels <- data.frame(
      id = paste0("rId", ids ),
      int_id = ids,
      type = rep("http://schemas.openxmlformats.org/officeDocument/2006/relationships/image", length(ids)),
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

  out_file <- normalizePath(path.expand(file), mustWork = FALSE, winslash = "/")

  unlink(out_file, force = TRUE)
  pack_folder(template_dir, out_file )
  unlink(template_dir, recursive = TRUE, force = TRUE)
  invisible()
}

