#ifndef _XH_XML_H_
#define _XH_XML_H_

#include "xh_config.h"
#include "xh_core.h"

extern const char indent_string[60];

XH_INLINE void
xh_xml_write_xml_declaration(xh_writer_t *writer, char *version, char *encoding)
{
    xh_buffer_t   *buf;
    size_t         ver_len, enc_len;

    buf     = &writer->main_buf;
    ver_len = strlen(version);
    enc_len = strlen(encoding);

    XH_WRITER_RESIZE_BUFFER(writer, buf, sizeof("<?xml version=\"\" encoding=\"\"?>\n") - 1 + ver_len * 6 + enc_len * 6)

    XH_BUFFER_WRITE_CONSTANT(buf, "<?xml version=\"")
    XH_BUFFER_WRITE_ESCAPE_ATTR(buf, version, ver_len);
    XH_BUFFER_WRITE_CONSTANT(buf, "\" encoding=\"")
    XH_BUFFER_WRITE_ESCAPE_ATTR(buf, encoding, enc_len);
    XH_BUFFER_WRITE_CHAR4(buf, "\"?>\n");

}

XH_INLINE void
xh_xml_write_node(xh_writer_t *writer, char *name, size_t name_len, SV *value, xh_bool_t raw)
{
    size_t         indent_len;
    xh_buffer_t   *buf;
    char          *content;
    STRLEN         content_len;

    buf     = &writer->main_buf;
    content = SvPV(value, content_len);

    if (writer->trim && content_len) {
        content = xh_str_trim(content, &content_len);
    }

    if (writer->indent) {
        indent_len = writer->indent_count * writer->indent;
        if (indent_len > sizeof(indent_string)) {
            indent_len = sizeof(indent_string);
        }

        /* "</" + "_" + ">" + "\n" */
        XH_WRITER_RESIZE_BUFFER(writer, buf, indent_len + name_len * 2 + 10 + (raw ? content_len : content_len * 5))

        XH_BUFFER_WRITE_LONG_STRING(buf, indent_string, indent_len);
    }
    else {
        /* "</" + "_" + ">" + "\n" */
        XH_WRITER_RESIZE_BUFFER(writer, buf, name_len * 2 + 10 + (raw ? content_len : content_len * 5))
    }

    XH_BUFFER_WRITE_CHAR(buf, '<')

    if (name[0] >= '0' && name[0] <= '9') {
        XH_BUFFER_WRITE_CHAR(buf, '_')
    }

    XH_BUFFER_WRITE_LONG_STRING(buf, name, name_len)

    XH_BUFFER_WRITE_CHAR(buf, '>')

    if (raw) {
        XH_BUFFER_WRITE_LONG_STRING(buf, content, content_len)
    }
    else {
        XH_BUFFER_WRITE_ESCAPE_STRING(buf, content, content_len)
    }

    XH_BUFFER_WRITE_CHAR2(buf, "</")

    if (name[0] >= '0' && name[0] <= '9') {
        XH_BUFFER_WRITE_CHAR(buf, '_')
    }

    XH_BUFFER_WRITE_LONG_STRING(buf, name, name_len)

    XH_BUFFER_WRITE_CHAR(buf, '>')

    if (writer->indent) {
        XH_BUFFER_WRITE_CHAR(buf, '\n')
    }
}

XH_INLINE void
xh_xml_write_empty_node(xh_writer_t *writer, char *name, size_t name_len)
{
    size_t       indent_len;
    xh_buffer_t *buf;

    buf = &writer->main_buf;

    if (writer->indent) {
        indent_len = writer->indent_count * writer->indent;
        if (indent_len > sizeof(indent_string)) {
            indent_len = sizeof(indent_string);
        }

        /* "</" + "_" + ">" + "\n" */
        XH_WRITER_RESIZE_BUFFER(writer, buf, indent_len + name_len + 5)

        XH_BUFFER_WRITE_LONG_STRING(buf, indent_string, indent_len);
    }
    else {
        /* "</" + "_" + ">" + "\n" */
        XH_WRITER_RESIZE_BUFFER(writer, buf, name_len + 5)
    }

    XH_BUFFER_WRITE_CHAR(buf, '<')

    if (name[0] >= '0' && name[0] <= '9') {
        XH_BUFFER_WRITE_CHAR(buf, '_')
    }

    XH_BUFFER_WRITE_LONG_STRING(buf, name, name_len)

    XH_BUFFER_WRITE_CHAR2(buf, "/>")

    if (writer->indent) {
        XH_BUFFER_WRITE_CHAR(buf, '\n')
    }
}

XH_INLINE void
xh_xml_write_start_node(xh_writer_t *writer, char *name, size_t name_len)
{
    size_t       indent_len;
    xh_buffer_t *buf;

    buf = &writer->main_buf;

    if (writer->indent) {
        indent_len = writer->indent_count++ * writer->indent;
        if (indent_len > sizeof(indent_string)) {
            indent_len = sizeof(indent_string);
        }

        /* "</" + "_" + ">" + "\n" */
        XH_WRITER_RESIZE_BUFFER(writer, buf, indent_len + name_len + 5)

        XH_BUFFER_WRITE_LONG_STRING(buf, indent_string, indent_len);
    }
    else {
        /* "</" + "_" + ">" + "\n" */
        XH_WRITER_RESIZE_BUFFER(writer, buf, name_len + 5)
    }

    XH_BUFFER_WRITE_CHAR(buf, '<')

    if (name[0] >= '0' && name[0] <= '9') {
        XH_BUFFER_WRITE_CHAR(buf, '_')
    }

    XH_BUFFER_WRITE_LONG_STRING(buf, name, name_len)

    XH_BUFFER_WRITE_CHAR(buf, '>')

    if (writer->indent) {
        XH_BUFFER_WRITE_CHAR(buf, '\n')
    }
}

XH_INLINE void
xh_xml_write_end_node(xh_writer_t *writer, char *name, size_t name_len)
{
    size_t         indent_len;
    xh_buffer_t   *buf;

    buf = &writer->main_buf;

    if (writer->indent) {
        indent_len = --writer->indent_count * writer->indent;
        if (indent_len > sizeof(indent_string)) {
            indent_len = sizeof(indent_string);
        }

        /* "</" + "_" + ">" + "\n" */
        XH_WRITER_RESIZE_BUFFER(writer, buf, indent_len + name_len + 5)

        XH_BUFFER_WRITE_LONG_STRING(buf, indent_string, indent_len);
    }
    else {
        /* "</" + "_" + ">" + "\n" */
        XH_WRITER_RESIZE_BUFFER(writer, buf, name_len + 5)
    }

    XH_BUFFER_WRITE_CHAR2(buf, "</")

    if (name[0] >= '0' && name[0] <= '9') {
        XH_BUFFER_WRITE_CHAR(buf, '_')
    }

    XH_BUFFER_WRITE_LONG_STRING(buf, name, name_len)

    XH_BUFFER_WRITE_CHAR(buf, '>')

    if (writer->indent) {
        XH_BUFFER_WRITE_CHAR(buf, '\n')
    }
}

XH_INLINE void
xh_xml_write_content(xh_writer_t *writer, SV *value)
{
    size_t         indent_len;
    xh_buffer_t   *buf;
    char          *content;
    size_t         content_len;
    STRLEN         str_len;

    buf         = &writer->main_buf;
    content     = SvPV(value, str_len);
    content_len = str_len;

    if (writer->trim) {
        content = xh_str_trim(content, &content_len);
    }

    if (writer->indent) {
        indent_len = writer->indent_count * writer->indent;
        if (indent_len > sizeof(indent_string)) {
            indent_len = sizeof(indent_string);
        }

        XH_WRITER_RESIZE_BUFFER(writer, buf, indent_len + content_len * 5)

        XH_BUFFER_WRITE_LONG_STRING(buf, indent_string, indent_len);
    }
    else {
        XH_WRITER_RESIZE_BUFFER(writer, buf, content_len * 5)
    }

    XH_BUFFER_WRITE_ESCAPE_STRING(buf, content, content_len);

    if (writer->indent) {
        XH_BUFFER_WRITE_CHAR(buf, '\n')
    }
}

XH_INLINE void
xh_xml_write_comment(xh_writer_t *writer, SV *value)
{
    size_t         indent_len;
    xh_buffer_t   *buf;
    char          *content;
    size_t         content_len;
    STRLEN         str_len;

    buf = &writer->main_buf;

    if (value == NULL) {
        content     = "";
        content_len = 0;
    }
    else {
        content     = SvPV(value, str_len);
        content_len = str_len;
    }

    if (writer->trim && content_len) {
        content = xh_str_trim(content, &content_len);
    }

    if (writer->indent) {
        indent_len = writer->indent_count * writer->indent;
        if (indent_len > sizeof(indent_string)) {
            indent_len = sizeof(indent_string);
        }

        /* "<!--" + "-->" */
        XH_WRITER_RESIZE_BUFFER(writer, buf, indent_len + content_len + 7)

        XH_BUFFER_WRITE_LONG_STRING(buf, indent_string, indent_len);
    }
    else {
        /* "<!--" + "-->" */
        XH_WRITER_RESIZE_BUFFER(writer, buf, content_len + 7)
    }

    XH_BUFFER_WRITE_CHAR4(buf, "<!--")
    XH_BUFFER_WRITE_LONG_STRING(buf, content, content_len);
    XH_BUFFER_WRITE_CHAR3(buf, "-->")

    if (writer->indent) {
        XH_BUFFER_WRITE_CHAR(buf, '\n')
    }
}

XH_INLINE void
xh_xml_write_cdata(xh_writer_t *writer, SV *value)
{
    size_t         indent_len;
    xh_buffer_t   *buf;
    char          *content;
    size_t         content_len;
    STRLEN         str_len;

    buf = &writer->main_buf;

    if (value == NULL) {
        content     = "";
        content_len = 0;
    }
    else {
        content     = SvPV(value, str_len);
        content_len = str_len;
    }

    if (writer->trim && content_len) {
        content = xh_str_trim(content, &content_len);
    }

    if (writer->indent) {
        indent_len = writer->indent_count * writer->indent;
        if (indent_len > sizeof(indent_string)) {
            indent_len = sizeof(indent_string);
        }

        /* "<![CDATA[" + "]]>" */
        XH_WRITER_RESIZE_BUFFER(writer, buf, indent_len + content_len + 12)

        XH_BUFFER_WRITE_LONG_STRING(buf, indent_string, indent_len);
    }
    else {
        /* "<![CDATA[" + "]]>" */
        XH_WRITER_RESIZE_BUFFER(writer, buf, content_len + 12)
    }

    XH_BUFFER_WRITE_CHAR9(buf, "<![CDATA[")
    XH_BUFFER_WRITE_LONG_STRING(buf, content, content_len);
    XH_BUFFER_WRITE_CHAR3(buf, "]]>")

    if (writer->indent) {
        XH_BUFFER_WRITE_CHAR(buf, '\n')
    }
}

XH_INLINE void
xh_xml_write_start_tag(xh_writer_t *writer, char *name, size_t name_len)
{
    size_t       indent_len;
    xh_buffer_t *buf;

    buf = &writer->main_buf;

    if (writer->indent) {
        indent_len = writer->indent_count * writer->indent;
        if (indent_len > sizeof(indent_string)) {
            indent_len = sizeof(indent_string);
        }

        /* "<" + "_" */
        XH_WRITER_RESIZE_BUFFER(writer, buf, indent_len + name_len + 2)

        XH_BUFFER_WRITE_LONG_STRING(buf, indent_string, indent_len);
    }
    else {
        /* "<" + "_" */
        XH_WRITER_RESIZE_BUFFER(writer, buf, name_len + 2)
    }

    XH_BUFFER_WRITE_CHAR(buf, '<')

    if (name[0] >= '0' && name[0] <= '9') {
        XH_BUFFER_WRITE_CHAR(buf, '_')
    }

    XH_BUFFER_WRITE_LONG_STRING(buf, name, name_len)
}

XH_INLINE void
xh_xml_write_end_tag(xh_writer_t *writer)
{
    xh_buffer_t *buf;

    buf = &writer->main_buf;

    XH_WRITER_RESIZE_BUFFER(writer, buf, 2)

    if (writer->indent) {
        XH_BUFFER_WRITE_CHAR2(buf, ">\n");
        writer->indent_count++;
    }
    else {
        XH_BUFFER_WRITE_CHAR(buf, '>')
    }
}

XH_INLINE void
xh_xml_write_closed_end_tag(xh_writer_t *writer)
{
    xh_buffer_t *buf;

    buf = &writer->main_buf;

    XH_WRITER_RESIZE_BUFFER(writer, buf, 3)

    if (writer->indent) {
        XH_BUFFER_WRITE_CHAR3(buf, "/>\n");
    }
    else {
        XH_BUFFER_WRITE_CHAR2(buf, "/>");
    }
}

XH_INLINE void
xh_xml_write_attribute(xh_writer_t *writer, char *name, size_t name_len, SV *value)
{
    xh_buffer_t   *buf;
    char          *content;
    size_t         content_len;
    STRLEN         str_len;

    buf = &writer->main_buf;

    if (value == NULL) {
        content     = "";
        content_len = 0;
    }
    else {
        content     = SvPV(value, str_len);
        content_len = str_len;
    }

    /* ' =""' */
    XH_WRITER_RESIZE_BUFFER(writer, buf, name_len + content_len * 6 + 4)

    XH_BUFFER_WRITE_CHAR(buf, ' ')

    XH_BUFFER_WRITE_LONG_STRING(buf, name, name_len)

    if (content_len == 0) {
        XH_BUFFER_WRITE_CHAR3(buf, "=\"\"");
    }
    else {
        XH_BUFFER_WRITE_CHAR2(buf, "=\"");
        XH_BUFFER_WRITE_ESCAPE_ATTR(buf, content, content_len);
        XH_BUFFER_WRITE_CHAR(buf, '"');
    }
}

#endif /* _XH_XML_H_ */
