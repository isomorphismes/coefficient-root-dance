/* Exercise the actual renderer's reset, scalar uploads and hit coordinates.
 * Only the GL transport is replaced; no complex or geometry implementation is.
 */
#include "../app/src/main/cpp/dance.c"

#include <assert.h>
#include <stdio.h>

static GLfloat uploaded_coefficients[MAX_POLYNOMIAL_DEGREE * 2];
static GLfloat uploaded_roots[MAX_POLYNOMIAL_DEGREE * 2];
static int upload_count;
static int draw_count;

static void near_float(float actual, float expected) {
    if (!isfinite(actual) || fabsf(actual - expected) >= 0.002f) {
        fprintf(stderr, "renderer scalar mismatch: actual %.9g; expected %.9g\n", actual, expected);
    }
    assert(isfinite(actual));
    assert(fabsf(actual - expected) < 0.002f);
}

void GL_APIENTRY glUseProgram(GLuint program) {
    assert(program == 17);
}

void GL_APIENTRY glUniform2f(GLint location, GLfloat x, GLfloat y) {
    assert(location == 101);
    near_float(x, 1280.0f);
    near_float(y, 720.0f);
}

void GL_APIENTRY glUniform1f(GLint location, GLfloat value) {
    assert(location == 102);
    near_float(value, 3.2f);
}

void GL_APIENTRY glUniform2fv(GLint location, GLsizei count, const GLfloat *value) {
    assert(count == MAX_POLYNOMIAL_DEGREE);
    assert(location == 103 || location == 104);
    GLfloat *destination = location == 103 ? uploaded_coefficients : uploaded_roots;
    for (int index = 0; index < MAX_POLYNOMIAL_DEGREE * 2; ++index) {
        destination[index] = value[index];
    }
    ++upload_count;
}

void GL_APIENTRY glUniform1i(GLint location, GLint value) {
    assert(location >= 105 && location <= 107);
    assert(value == (location == 105 ? 2 : location == 106 ? SELECTION_NONE : -1));
}

void GL_APIENTRY glBindVertexArray(GLuint array) {
    assert(array == 3);
}

void GL_APIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count) {
    assert(mode == GL_TRIANGLES && first == 0 && count == 3);
    ++draw_count;
}

EGLBoolean EGLAPIENTRY eglSwapBuffers(EGLDisplay display, EGLSurface surface) {
    assert(display == (EGLDisplay)(uintptr_t)1);
    assert(surface == (EGLSurface)(uintptr_t)2);
    return EGL_TRUE;
}

EGLint EGLAPIENTRY eglGetError(void) {
    assert(!"the successful capture must not query an EGL error");
    return EGL_SUCCESS;
}

int __android_log_print(int priority, const char *tag, const char *format, ...) {
    (void)priority;
    (void)tag;
    (void)format;
    assert(!"the successful capture must not log an EGL failure");
    return 0;
}

static void check_screen(
    const struct engine *engine,
    float complex value,
    bool right_side,
    float expected_x,
    float expected_y
) {
    float x = 0.0f;
    float y = 0.0f;
    complex_to_screen(engine, value, right_side, &x, &y);
    near_float(x, expected_x);
    near_float(y, expected_y);
}

int main(void) {
    struct engine engine = {0};
    reset_polynomial(&engine);
    engine.display = (EGLDisplay)(uintptr_t)1;
    engine.surface = (EGLSurface)(uintptr_t)2;
    engine.width = 1280;
    engine.height = 720;
    engine.program = 17;
    engine.vao = 3;
    engine.resolution_location = 101;
    engine.half_height_location = 102;
    engine.coefficients_location = 103;
    engine.roots_location = 104;
    engine.degree_location = 105;
    engine.active_kind_location = 106;
    engine.active_index_location = 107;

    draw_frame(&engine);
    assert(upload_count == 2 && draw_count == 1 && !engine.dirty);

    const float expected_coefficients[] = {-0.76f, 1.0f, 0.0f, -0.2f};
    const float expected_roots[] = {-1.0f, 0.6f, 1.0f, -0.4f};
    for (int index = 0; index < MAX_POLYNOMIAL_DEGREE * 2; ++index) {
        near_float(uploaded_coefficients[index], index < 4 ? expected_coefficients[index] : 0.0f);
        near_float(uploaded_roots[index], index < 4 ? expected_roots[index] : 0.0f);
    }

    check_screen(&engine, engine.coefficients[0], false, 234.5f, 247.5f);
    check_screen(&engine, engine.coefficients[1], false, 320.0f, 382.5f);
    check_screen(&engine, engine.roots[0], true, 847.5f, 292.5f);
    check_screen(&engine, engine.roots[1], true, 1072.5f, 405.0f);
    assert(nearest_handle(&engine, 235.0f, 248.0f, SELECTION_COEFFICIENT) == 0);
    assert(nearest_handle(&engine, 320.0f, 383.0f, SELECTION_COEFFICIENT) == 1);
    assert(nearest_handle(&engine, 847.0f, 292.0f, SELECTION_ROOT) == 0);
    assert(nearest_handle(&engine, 1072.0f, 405.0f, SELECTION_ROOT) == 1);
    puts("actual renderer Cartesian uploads and initial drag coordinates passed");
    return 0;
}
