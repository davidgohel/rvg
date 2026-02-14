#' @useDynLib rvg,.registration = TRUE
#' @importFrom Rcpp sourceCpp
#' @import systemfonts

r_font_families <- c("sans", "serif", "mono", "symbol")

#' @importFrom gdtools font_family_exists font_set_auto
validate_fonts <- function(system_fonts = list()) {
  auto <- font_set_auto()
  defaults <- c(
    sans = auto$sans,
    serif = auto$serif,
    mono = auto$mono,
    symbol = auto$symbol
  )
  found <- unlist(lapply(system_fonts, font_family_exists))
  if (length(system_fonts) > 0 && any(!found)) {
    not_found <- names(system_fonts)[!found]
    warning(
      "Font families not found on this system and replaced by defaults: ",
      paste0('"', not_found, '"', collapse = ", "),
      ". Use gdtools::font_family_exists() to check availability.",
      call. = FALSE
    )
  }
  system_fonts <- system_fonts[found]
  missing_fonts <- setdiff(r_font_families, names(system_fonts))
  system_fonts[missing_fonts] <- defaults[missing_fonts]
  system_fonts
}
