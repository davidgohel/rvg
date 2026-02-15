context("XLSX raster generation")
library(grid)
library(xml2)

test_that("raster is generated", {
  file <- tempfile()
  dml_xlsx(file = file, bg = "transparent")
  pushViewport(viewport(width = 0.8, height = 0.5, name = "vp1"))
  grid.rect()
  pushViewport(viewport(width = 0.8, height = 0.5, name = "vp2", angle = 10))
  redGradient <- matrix(hcl(0, 80, seq(50, 80, 10)), nrow = 4, ncol = 5)
  grid.raster(redGradient)
  dev.off()

  xml_raw <- paste0(readLines(file, warn = FALSE), collapse = "")
  m <- regmatches(xml_raw, regexpr("<!-- rvg_raster_prefix:(.+?) -->", xml_raw, perl = TRUE))
  expect_length(m, 1L)
  raster_prefix <- sub("<!-- rvg_raster_prefix:(.+?) -->", "\\1", m, perl = TRUE)
  raster_files <- list.files(
    path = dirname(raster_prefix),
    pattern = paste0("^", basename(raster_prefix), "(.*)\\.png$"),
    full.names = TRUE
  )
  expect_equal(length(raster_files), expected = 1)
})


test_that("pic tag can be found", {
  file <- tempfile()

  dml_xlsx(file = file, bg = "transparent")
  pushViewport(viewport(width = 0.8, height = 0.5, name = "vp1"))
  grid.rect()
  pushViewport(viewport(width = 0.8, height = 0.5, name = "vp2", angle = 10))
  redGradient <- matrix(hcl(0, 80, seq(50, 80, 10)), nrow = 4, ncol = 5)
  grid.raster(redGradient)
  dev.off()

  doc <- read_xml(file)
  pic_node <- xml_find_first(doc, ".//xdr:pic", ns = xml_ns(doc))
  expect_is(pic_node, "xml_node")

  blip_node <- xml_find_first(pic_node, ".//a:blip", ns = xml_ns(doc))
  expect_is(blip_node, "xml_node")
})

