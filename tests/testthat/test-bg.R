context("Background color")
library(xml2)

test_that("pptx background exists if background is not transparent", {

  file <- tempfile()
  dml_pptx( file = file, bg = "#123456" )
  plot.new()
  dev.off()


  doc <- read_xml(file)
  solid_fill <- xml_find_first(doc, ".//p:sp/p:spPr/a:solidFill", ns = xml_ns( doc ))
  expect_is(object = solid_fill, class = "xml_node")

  color_node <- xml_find_first(doc, ".//p:sp/p:spPr/a:solidFill/a:srgbClr", ns = xml_ns( doc ))
  color <- xml_attr(color_node, "val")
  expect_equal(object = color, expected = "123456")

  alpha_node <- xml_find_first(doc, ".//p:sp/p:spPr/a:solidFill/a:srgbClr/a:alpha", ns = xml_ns( doc ))
  alpha <- xml_attr(alpha_node, "val")
  expect_equal(object = alpha, expected = "100000")

  file <- tempfile()
  dml_pptx( file = file, bg = "#12345699" )
  plot.new()
  dev.off()

  doc <- read_xml(file)
  alpha_node <- xml_find_first(doc, ".//p:sp/p:spPr/a:solidFill/a:srgbClr/a:alpha", ns = xml_ns( doc ))
  alpha <- xml_attr(alpha_node, "val")
  expect_equal(object = alpha, expected = "60000")

})


test_that("pptx background does not exist if background is transparent", {

  file <- tempfile()
  dml_pptx( file = file, bg = "transparent" )
  plot.new()
  dev.off()

  doc <- read_xml(file)
  bg_node <- xml_find_first(doc, ".//p:sp", ns = xml_ns( doc ))
  expect_is(bg_node, class = "xml_missing")
})


test_that("docx background exists if background is not transparent", {

  file <- tempfile()
  dml_docx( file = file, bg = "#123456" )
  plot.new()
  dev.off()


  doc <- read_xml(file)
  solid_fill <- xml_find_first(doc, ".//wps:wsp/wps:spPr/a:solidFill", ns = xml_ns( doc ))
  expect_is(solid_fill, class = "xml_node")

  color_node <- xml_find_first(doc, ".//wps:wsp/wps:spPr/a:solidFill/a:srgbClr", ns = xml_ns( doc ))
  color <- xml_attr(color_node, "val")
  expect_equal(object = color, expected = "123456")

  alpha_node <- xml_find_first(doc, ".//wps:wsp/wps:spPr/a:solidFill/a:srgbClr/a:alpha", ns = xml_ns( doc ))
  alpha <- xml_attr(alpha_node, "val")
  expect_equal(object = alpha, expected = "100000")

  file <- tempfile()
  dml_docx( file = file, bg = "#12345699" )
  plot.new()
  dev.off()

  doc <- read_xml(file)
  alpha_node <- xml_find_first(doc, ".//wps:wsp/wps:spPr/a:solidFill/a:srgbClr/a:alpha", ns = xml_ns( doc ))
  alpha <- xml_attr(alpha_node, "val")
  expect_equal(object = alpha, expected = "60000")
})


test_that("docx background does not exist if background is transparent", {

  file <- tempfile()
  dml_docx( file = file, bg = "transparent" )
  plot.new()
  dev.off()

  doc <- read_xml(file)
  bg_node <- xml_find_first(doc, ".//wps:wsp", ns = xml_ns( doc ))
  expect_is(bg_node, class = "xml_missing")
})

