context("add vg in pptx/docx")
library(xml2)
library(officer)

test_that("ph_with_vg", {
  doc <- read_pptx() %>%
    add_slide("Title and Content", "Office Theme") %>%
    ph_with_vg(code = barplot(1:5, col = 2:6), type = "body")

  xmldoc <- doc$slide$get_slide(1)$get()
  node <- xml_find_all(xmldoc, "//p:spTree/p:grpSp/*")
  expect_false( inherits(node, "xml_missing") )
})

test_that("body_add_vg", {
  x <- read_docx() %>%
    body_add_vg(code = barplot(1:5, col = 2:6) )

  node <- x$doc_obj$get_at_cursor()
  wgp <- xml_child(node, "/w:r/w:drawing/wp:inline/a:graphic/a:graphicData/wpg:wgp")

  expect_is( wgp, "xml_node" )
})

