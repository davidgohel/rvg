context("PPTX text")
library(xml2)

test_that("text can be found", {

  file <- tempfile()
  dml_pptx( file = file, bg = "transparent" )
  plot.new()
  text(0.2, 0.2, "hello")
  dev.off()

  doc <- read_xml(file)
  text_node <- xml_find_one(doc, ".//p:sp/p:txBody/a:p/a:r/a:t", ns = xml_ns( doc ))
  expect_is(object = text_node, class = "xml_node")
  expect_equal(xml_text(text_node), "hello")
})

