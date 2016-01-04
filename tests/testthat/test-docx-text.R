context("DOCX text")
library(xml2)

test_that("text can be found", {

  file <- tempfile()
  dml_docx( file = file, bg = "transparent" )
  plot.new()
  text(0.2, 0.2, "hello")
  dev.off()

  doc <- read_xml(file)
  text_node <- xml_find_one(doc, ".//wps:wsp/wps:txbx/w:txbxContent/w:p/w:r/w:t", ns = xml_ns( doc ))
  expect_is(object = text_node, class = "xml_node")
  expect_equal(xml_text(text_node), "hello")
})

test_that("text alpha col can be found", {

  file <- tempfile()
  dml_docx( file = file, bg = "transparent" )
  plot.new()
  text(0.2, 0.2, "hello", col = "#00000099")
  dev.off()

  doc <- scan(file, what = "character", sep = "\n", quiet = TRUE)
  expect_true(grepl("<w14:alpha val=\"60000\"/>", doc))
})

