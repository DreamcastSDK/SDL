/* list of OpenGL functions sorted alphabetically
   If you need to use a GL function from the SDL video subsystem,
   change it's entry from SDL_PROC_UNUSED to SDL_PROC and rebuild.
*/
#define SDL_PROC_UNUSED(ret,func,params)

SDL_PROC(void, glBindTexture, (GLenum, GLuint))
SDL_PROC(void, glBlendFunc, (GLenum, GLenum))
SDL_PROC(void, glClear, (GLbitfield))
SDL_PROC(void, glClearColor, (GLclampf, GLclampf, GLclampf, GLclampf))
SDL_PROC(void, glColor4f, (GLfloat, GLfloat, GLfloat, GLfloat))
SDL_PROC(void, glDeleteTextures, (GLsizei n, const GLuint * textures))
SDL_PROC(void, glDisable, (GLenum cap))
SDL_PROC(void, glEnable, (GLenum cap))
SDL_PROC(void, glGenTextures, (GLsizei n, GLuint * textures))
SDL_PROC(GLenum, glGetError, (void))
SDL_PROC(void, glGetIntegerv, (GLenum pname, GLint * params))
SDL_PROC(void, glOrthof,
         (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top,
          GLfloat zNear, GLfloat zFar))
SDL_PROC(void, glTexImage2D,
         (GLenum target, GLint level, GLint internalformat, GLsizei width,
          GLsizei height, GLint border, GLenum format, GLenum type,
          const GLvoid * pixels))
SDL_PROC(void, glViewport, (GLint x, GLint y, GLsizei width, GLsizei height))

SDL_PROC(void, glMatrixMode, (GLenum mode))
SDL_PROC(void, glLoadIdentity, (void))

SDL_PROC(void, glTexParameteri, (GLenum target, GLenum pname, GLint param))

SDL_PROC(void, glPixelStorei, (GLenum pname, GLint param))

SDL_PROC(void, glTexSubImage2D,\
		 (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels))

SDL_PROC(void, glTexEnvf, (GLenum target, GLenum pname, GLfloat param))

SDL_PROC(void, glPopMatrix, (void))
SDL_PROC(void, glPushMatrix, (void))
SDL_PROC(void, glVertexPointer, (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer))
SDL_PROC(void, glDrawArrays, (GLenum mode, GLint first, GLsizei count))
SDL_PROC(void, glDisableClientState, (GLenum array))
SDL_PROC(void, glEnableClientState, (GLenum array))
SDL_PROC(void, glTexCoordPointer, (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer))
SDL_PROC(void, glScissor, (GLint x, GLint y, GLsizei width, GLsizei height))
SDL_PROC(void, glGetFloatv, (GLenum pname, GLfloat *params))
SDL_PROC(void, glTexParameteriv, (GLenum target, GLenum pname, const GLint *params))
SDL_PROC(void, glDrawTexiOES, (GLint x, GLint y, GLint z, GLint width, GLint height))

/* vi: set ts=4 sw=4 expandtab: */
