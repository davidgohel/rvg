#' @importFrom utils unzip
#' @import xml2
#' @title Microsoft PowerPoint Graphics Device
#'
#' @description
#' A graphical device for Microsoft PowerPoint documents.
#' @param file filename of the Microsoft PowerPoint document to produce. File
#' extension must be \code{.pptx}.
#' @param fun Plotting code to execute
#' @param ... arguments for \code{fun} (passed on to \code{\link{dml_docx}}.)
#' @param height,width Height and width in inches.
#' @param pagesize Word document page size in inches.
#' A named vector (\code{width} and \code{height}).
#' @param margins Word document margins size in inches.
#' A named vector (\code{left}, \code{right}, \code{top}, \code{bottom}).
#' @param bg Default background color for the plot (defaults to "white").
#' @param fontname_serif,fontname_sans,fontname_mono,fontname_symbol font
#' names for font faces
#' @param pointsize default point size.
#' @param editable should vector graphics elements (points, text, etc.)
#' be editable.
#' @examples
#' pptx_device(file = "my_plot_1.pptx", fun = function() plot(rnorm(10)))
#' pptx_device(file = "my_plot_2.pptx", fun = function() barplot(1:7, col = 1:7))
#' @keywords device
#' @export
pptx_device <- function(
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
  unzip( zipfile = file.path( system.file(package = "rvg"), "templates/vanilla.pptx" ), exdir = template_dir )
  drop_dir <- file.path(template_dir, "__MACOSX")
  if( dir.exists(drop_dir) ) unlink(drop_dir, force = TRUE, recursive = TRUE)

  document_rel <- file.path( template_dir, "ppt/slides/", "_rels/slide1.xml.rels" )
  relationships <- read_relationship( filename = document_rel )

  uid <- basename(tempfile(pattern = ""))
  img_directory = file.path(getwd(), uid )

  dml_file <- tempfile()
  dml_pptx(file = dml_file, width = width, height = height, bg = bg,
           fontname_serif = fontname_serif, fontname_sans = fontname_sans,
           fontname_mono = fontname_mono, fontname_symbol = fontname_symbol,
           pointsize = pointsize, editable = editable,
           id = 0L,
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
  # cat("xmlns:pic=\"http://schemas.openxmlformats.org/drawingml/2006/picture\"")
  cat(">")

  cat("<p:cSld>")
  cat(dml_str)
  cat("</p:cSld>")
  cat("</p:sld>")
  sink()

  out_file <- normalizePath(path.expand(file), mustWork = FALSE, winslash = "/")

  unlink(out_file, force = TRUE)
  pack_folder(template_dir, out_file )
  # unlink(template_dir, recursive = TRUE, force = TRUE)
  template_dir
}

