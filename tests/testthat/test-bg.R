context("Background color")
library(xml2)

test_that("pptx background exists if background is not transparent", {

  file <- tempfile()
  dml_pptx( file = file, bg = "#123456" )
  plot.new()
  dev.off()


  doc <- read_xml(file)
  solid_fill <- try( xml_find_one(doc, ".//p:sp/p:spPr/a:solidFill", ns = xml_ns( doc )), silent = FALSE )
  expect_is(object = solid_fill, class = "xml_node")

  color_node <- xml_find_one(doc, ".//p:sp/p:spPr/a:solidFill/a:srgbClr", ns = xml_ns( doc ))
  color <- xml_attr(color_node, "val")
  expect_equal(object = color, expected = "123456")

  alpha_node <- xml_find_one(doc, ".//p:sp/p:spPr/a:solidFill/a:srgbClr/a:alpha", ns = xml_ns( doc ))
  alpha <- xml_attr(alpha_node, "val")
  expect_equal(object = alpha, expected = "100000")

  file <- tempfile()
  dml_pptx( file = file, bg = "#12345699" )
  plot.new()
  dev.off()

  doc <- read_xml(file)
  alpha_node <- xml_find_one(doc, ".//p:sp/p:spPr/a:solidFill/a:srgbClr/a:alpha", ns = xml_ns( doc ))
  alpha <- xml_attr(alpha_node, "val")
  expect_equal(object = alpha, expected = "60000")

})


test_that("pptx background does not exist if background is transparent", {

  file <- tempfile()
  dml_pptx( file = file, bg = "transparent" )
  plot.new()
  dev.off()


  doc <- read_xml(file)
  bg_node <- try( xml_find_one(doc, ".//p:sp", ns = xml_ns( doc )), silent = TRUE )
  expect_true( inherits(bg_node, "try-error") )

})
