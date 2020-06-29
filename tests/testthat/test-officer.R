context("add vg in pptx")
library(xml2)
library(officer)

test_that("ph_with_vg", {
  doc <- read_pptx()
  doc <- add_slide(doc, "Title and Content", "Office Theme")
  doc <- ph_with(doc, dml(code = barplot(1:5, col = 2:6)), location = ph_location_type(type = "body"))

  xmldoc <- doc$slide$get_slide(1)$get()
  node <- xml_find_all(xmldoc, "//p:spTree/p:grpSp/*")
  expect_false( inherits(node, "xml_missing") )
})


