#include "plugin.h"

ZATHURA_PLUGIN_REGISTER_WITH_FUNCTIONS(
  "txt",
  0, 1, 0,
  ZATHURA_PLUGIN_FUNCTIONS({
    .document_open     = txt_document_open,
    .document_free     = txt_document_free,
    .page_init         = txt_page_init,
    .page_clear        = txt_page_clear,
    .page_render_cairo = txt_page_render_cairo,
  }),
  ZATHURA_PLUGIN_MIMETYPES({
    "text/plain"
  })
)

zathura_error_t txt_document_open(zathura_document_t *document) {
    txt_document_t *txt_document = g_malloc0(sizeof(txt_document_t));
    if (txt_document == NULL) {
        return ZATHURA_ERROR_OUT_OF_MEMORY;
    }

    const char *path = zathura_document_get_path(document);
    FILE *f_in = fopen(path, "r");
    if (f_in == NULL) {
        g_free(txt_document);
        return ZATHURA_ERROR_UNKNOWN;
    }

    txt_document->pages = (txt_page_t *)malloc(sizeof(txt_page_t));
    if (txt_document->pages == NULL) {
        fclose(f_in);
        g_free(txt_document);
        return ZATHURA_ERROR_OUT_OF_MEMORY;
    }

    txt_document->pages[0].data = malloc(sizeof(char *) * 4095);
    if (txt_document->pages[0].data == NULL) {
        fclose(f_in);
        free(txt_document->pages);
        g_free(txt_document);
        return ZATHURA_ERROR_OUT_OF_MEMORY;
    }

    char buffer[4096];
    if (fgets(buffer, sizeof(buffer), f_in) != NULL) {
        txt_document->pages[0].data[0] = g_strdup(buffer);
    } else {
        txt_document->pages[0].data[0] = g_strdup("");
    }
    fclose(f_in);

    zathura_document_set_number_of_pages(document, 1);
    zathura_document_set_data(document, txt_document);

    return ZATHURA_ERROR_OK;
}

zathura_error_t txt_document_free(zathura_document_t *document, void *txt_document) {
    if (document == NULL || txt_document == NULL) {
        return ZATHURA_ERROR_UNKNOWN;
    }
    
    txt_document_t *txt_doc = (txt_document_t *)txt_document;
    if (txt_doc->pages && txt_doc->pages[0].data) {
        free(txt_doc->pages[0].data[0]);
        free(txt_doc->pages[0].data);
    }
    free(txt_doc->pages);
    g_free(txt_doc);

    return ZATHURA_ERROR_OK;
}

zathura_error_t txt_page_init(zathura_page_t *page) {
    zathura_document_t *document = zathura_page_get_document(page);
    if (document == NULL) return ZATHURA_ERROR_UNKNOWN;

    txt_document_t *txt_document = zathura_document_get_data(document);
    zathura_page_set_width(page, 500);
    zathura_page_set_height(page, 500);
    zathura_page_set_data(page, txt_document->pages);

    return ZATHURA_ERROR_OK;
}

zathura_error_t txt_page_clear(zathura_page_t *page, void *txt_page) {
    if (page == NULL || txt_page == NULL) {
        return ZATHURA_ERROR_UNKNOWN;
    }

    txt_page_t *txt_pg = (txt_page_t *)txt_page;
    if (txt_pg->data) {
        free(txt_pg->data[0]);
        free(txt_pg->data);
    }

    return ZATHURA_ERROR_OK;
}

zathura_error_t txt_page_render_cairo(zathura_page_t *page, void *txt_page, cairo_t *cairo, bool printing) {
    (void) printing;

    if (page == NULL || txt_page == NULL) {
        return ZATHURA_ERROR_UNKNOWN;
    }

    zathura_document_t *document = zathura_page_get_document(page);
    if (document == NULL) return ZATHURA_ERROR_UNKNOWN;

    txt_page_t *txt_pg = (txt_page_t *)txt_page;
    if (txt_pg->data[0] != NULL) {
        cairo_move_to(cairo, 50, 50);
        cairo_show_text(cairo, txt_pg->data[0]);
    }
    
    return ZATHURA_ERROR_OK;
}
