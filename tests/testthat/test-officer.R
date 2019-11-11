context("add vg in pptx/docx")
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

test_that("body_add_vg", {
  x <- read_docx()
  x <- body_add_vg(x, code = barplot(1:5, col = 2:6) )

  node <- x$doc_obj$get_at_cursor()
  wgp <- xml_child(node, "/w:r/w:drawing/wp:inline/a:graphic/a:graphicData/wpg:wgp")

  expect_is( wgp, "xml_node" )
})

