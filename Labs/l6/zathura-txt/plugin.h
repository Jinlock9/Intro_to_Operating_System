#ifndef PLUGIN_H
#define PLUGIN_H

#include <zathura/plugin-api.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <stdlib.h>
#include <cairo.h>
#include <stdbool.h>

typedef struct txt_page_s {
  char **data;
} txt_page_t;

typedef struct txt_document_s {
  struct txt_page_s *pages;
} txt_document_t;

zathura_error_t txt_document_open(zathura_document_t *document);
zathura_error_t txt_document_free(zathura_document_t *document, void *txt_document);
zathura_error_t txt_page_init(zathura_page_t *page);
zathura_error_t txt_page_clear(zathura_page_t *page, void *txt_page);
zathura_error_t txt_page_render_cairo(zathura_page_t *page, void *txt_page, cairo_t *cairo, bool printing);

#endif // PLUGIN_H