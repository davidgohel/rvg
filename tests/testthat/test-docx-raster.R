context("DOCX raster generation")
library(grid)
library(xml2)

test_that("raster is generated", {

  file <- tempfile()
  uid <- basename( tempfile( ))
  dml_docx( file = file, raster_prefix = uid, bg = "transparent" )
  pushViewport(viewport(width=0.8, height=0.5, name="vp1"))
  grid.rect( )
  pushViewport(viewport(width=0.8, height=0.5, name="vp2",angle=10))
  redGradient <- matrix(hcl(0, 80, seq(50, 80, 10)), nrow=4, ncol=5)
  grid.raster(redGradient )
  dev.off()

  raster_files <- list.files(pattern = paste0("^", uid, "(.*)\\.png$"), full.names = TRUE )
  expect_equal(length(raster_files), expected = 1)
})

test_that("docx raster is in media", {
  doc_file <- write_docx(file = "doc.docx",
             code = {
               pushViewport(viewport(width = 0.8, height = 0.5, name = "vp1"))
               grid.rect()
               pushViewport(viewport( width = 0.8, height = 0.5, name = "vp2",angle = 10 ))
               redGradient <- matrix(hcl(0, 80, seq(50, 80, 10)), nrow = 4, ncol = 5)
               grid.raster(redGradient)
             })
  expect_true(file.exists(doc_file))
  utils::unzip( zipfile = doc_file, exdir = "doc_content" )
  expect_true( length( list.files( file.path("doc_content", "word/media")) ) == 1 )
})

test_that("pptx raster is in media", {
  doc_file <- write_pptx(file = "doc.pptx",
                         code = {
                           pushViewport(viewport(width = 0.8, height = 0.5, name = "vp1"))
                           grid.rect()
                           pushViewport(viewport( width = 0.8, height = 0.5, name = "vp2",angle = 10 ))
                           redGradient <- matrix(hcl(0, 80, seq(50, 80, 10)), nrow = 4, ncol = 5)
                           grid.raster(redGradient)
                         })
  expect_true(file.exists(doc_file))
  utils::unzip( zipfile = doc_file, exdir = "pres_content" )
  expect_true( length( list.files( file.path("pres_content", "ppt/media")) ) == 1 )
})

test_that("pic tag can be found", {

  file <- tempfile()

  dml_docx( file = file, bg = "transparent" )
  pushViewport(viewport(width=0.8, height=0.5, name="vp1"))
  grid.rect( )
  pushViewport(viewport(width=0.8, height=0.5, name="vp2",angle=10))
  redGradient <- matrix(hcl(0, 80, seq(50, 80, 10)), nrow=4, ncol=5)
  grid.raster(redGradient )
  dev.off()


  doc <- read_xml(file)
  pic_node <- xml_find_first(doc, ".//a:blipFill", ns = xml_ns( doc ))
  expect_is( pic_node, "xml_node")

  blip_node <- xml_find_first(pic_node, ".//a:blip", ns = xml_ns( doc ))
  expect_is( blip_node, "xml_node")
})

unlink(list.files(pattern = "\\.png$"), force = TRUE)
unlink(list.files(pattern = "\\.docx$"), force = TRUE)
unlink(list.files(pattern = "\\.pptx$"), force = TRUE)
unlink("doc_content", recursive = TRUE, force = TRUE)
unlink("pres_content", recursive = TRUE, force = TRUE)
