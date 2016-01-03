context("Extension filenames")
library(xml2)

test_that("check docx extension", {

  file_abs_path <- try( write_docx(file = "my_plot.docx", code = plot(rnorm(10))), silent = TRUE )
  expect_is(object = file_abs_path, class = "character")

  file_abs_path <- try( write_docx(file = "my_plot.docxa", code = plot(rnorm(10))), silent = TRUE )
  expect_is(object = file_abs_path, class = "try-error")
})

test_that("check pptx extension", {

  file_abs_path <- try( write_pptx(file = "my_plot.pptx", code = plot(rnorm(10))), silent = TRUE )
  expect_is(object = file_abs_path, class = "character")

  file_abs_path <- try( write_pptx(file = "my_plot.pptxa", code = plot(rnorm(10))), silent = TRUE )
  expect_is(object = file_abs_path, class = "try-error")
})

unlink(list.files(pattern = "^my_plot"), force = TRUE)
