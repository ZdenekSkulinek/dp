/* SoundAnalyzer
 *
 * This program demonstrates usage of OpenCL for signal processing.
 * Simply computes spectrum of signal with Goertzel algorithm.
 *
 * Copyright (C) 2017 Zdeněk Skulínek
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include "cbargraphrenderer.h"
#include "capplication.h"
#include <QOpenGLFunctions>
#include <QTime>
#include <chrono>

#define TEST_ERROR(_msg)		\
    error = glGetError();		\
    if (error != GL_NO_ERROR) {	\
        qWarning()<<" GL error (hex)="<<hex<<error<<"  :"<<_msg<<"\n";	\
         ;		\
    }

void CBarGraphRenderer::printShaderInfoLog(GLuint obj)
{
  int infologLength = 0;
  int charsWritten = 0;
  char *infoLog;

  glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

  if (infologLength > 0)
  {
    infoLog = (char *)malloc(infologLength);
    glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
    qWarning()<<infoLog;
    free(infoLog);
  }
}

void CBarGraphRenderer::printProgramInfoLog(GLuint obj)
{
  int infologLength = 0;
  int charsWritten = 0;
  char *infoLog;

  glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

  if (infologLength > 0)
  {
    infoLog = (char *)malloc(infologLength);
    glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
    qWarning()<<infoLog;
    free(infoLog);
  }
}

CBarGraphRenderer::~CBarGraphRenderer()
{
    resetInputQueue();
    //delete m_program;
    glDetachShader( m_program, m_vertexShader );
    glDetachShader( m_program, m_fragmentShader );
    glDeleteShader( m_vertexShader );
    glDeleteShader( m_fragmentShader );
    glDeleteProgram( m_program );
    m_program = GLuint(-1);
}

unsigned char* CBarGraphRenderer::getDataForMonitor()
{
    return m_oldData;
}



void CBarGraphRenderer::setInputQueue(CInterThreadQueue* queue)
{
    if ( m_inputQueue != queue ){


        m_inputQueue = queue;
    }

}

void CBarGraphRenderer::setLength(int length) {
    if ( length==0 ) {
        return;
    }
    if(m_iLength==length){

        return;
    }
    m_iLength = length;

    if (m_arrCoords) delete m_arrCoords;
    m_arrCoords = new float[ ((length + 1) / 1 ) * 6 ];
    if ( !m_arrCoords ) {

        CApplication::displayWarning(std::string("Rendered Error"),std::string("No memory"));
    }
}

void CBarGraphRenderer::resetInputQueue()
{

    if( m_oldData ) {
        m_inputQueue->free(m_oldData);
        m_oldData = nullptr;
    }
}

void CBarGraphRenderer::paint()
{
    GLuint error;

    if ( m_program == GLuint(-1) ) {

        initializeOpenGLFunctions();

        if ( m_majorVersion == -1 )
        {
            glGetIntegerv(GL_MAJOR_VERSION, &m_majorVersion);
            glGetIntegerv(GL_MINOR_VERSION, &m_minorVersion);
        }

        m_vertexShader = glCreateShader( GL_VERTEX_SHADER );
        TEST_ERROR (" Create vertex shader");

        std::string vertexSource=CApplication::loadFromResources(":/graph_vertex.glsl");
        const char *cvertexSource = vertexSource.c_str();
        glShaderSource( m_vertexShader , 1 , &cvertexSource, NULL);
        TEST_ERROR (" Create vertex shader source");
        glCompileShader( m_vertexShader );
        printShaderInfoLog( m_vertexShader );

        m_fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
        TEST_ERROR (" Create fragment shader");
        std::string fragmentSource = CApplication::loadFromResources(":/graph_fragment.glsl");
        const char * cfragmentSource = fragmentSource.c_str();
        glShaderSource( m_fragmentShader , 1 , &cfragmentSource, NULL );
        TEST_ERROR (" Create fragment shader source");
        glCompileShader( m_fragmentShader );
        printShaderInfoLog( m_fragmentShader );

        m_program = glCreateProgram();
        TEST_ERROR (" Create m_program");
        glAttachShader(m_program,m_vertexShader);
        TEST_ERROR (" Create m_program attach vertex");
        glAttachShader(m_program,m_fragmentShader);
        TEST_ERROR (" Create m_program attach fragment");

        glLinkProgram(m_program);
        printProgramInfoLog(m_program);


    }

    glEnable(GL_SCISSOR_TEST);
    glScissor( m_itemOrigin.x(),
               m_viewportSize.height() - m_itemOrigin.y() - m_itemSize.height(),
               m_itemSize.width(),
               m_itemSize.height());
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    glUseProgram(m_program);
    printProgramInfoLog(m_program);
    glEnableVertexAttribArray(glGetAttribLocation(m_program, "vertices"));

    short sinus[512];
    for(int i=0;i<512;i++) sinus[i] = short( sin( i * 2 * M_PI / 512) * 0x7FFF );
    unsigned char* data = nullptr;

//    static auto begin = std::chrono::high_resolution_clock::now();
//    static long bytes = 0;

    if ( m_parent ) {

        data = m_parent->getDataForMonitor();
    }
    else {
        if ( m_inputQueue ) {
            unsigned char* dataTemp;
            while( 1 ) {
                dataTemp = (unsigned char*)m_inputQueue->peek();
                if ( !dataTemp ) break;

                //            if ( !m_oldData ) {

                //                begin = std::chrono::high_resolution_clock::now();
                //                bytes = 0;
                //            }
                //            auto end = std::chrono::high_resolution_clock::now();
                //            bytes+=512;
                //            long duration = std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count();
                //            qDebug()<<" FF "<<duration<<" / "<<bytes<<" / "<< ( duration != 0? bytes *1000000 / duration : 0);

                if ( m_oldData ) m_inputQueue->free( (unsigned char*)m_oldData );
                m_oldData = dataTemp;
                data = dataTemp;
            }
            data = m_oldData;
        }
    }
    if( !data ) {

        if ( !m_oldData ) {

            data = (unsigned char *)sinus;
            m_iStride = 0;
            m_iBytesPerSample = 2;
            m_iOffset = 0;
            setLength(512);
        }
    }

    if( !m_arrCoords ) {

        return;
    }

    GLint attVetices = glGetAttribLocation(m_program, "vertices");
    GLint attPM = glGetUniformLocation(m_program, "mvp_matrix");
    GLint attC = glGetUniformLocation(m_program, "color");

    GLfloat color[] = {1.0,0.0,0.0,1.0};
    qreal colord[]= {0.0,0.0,0.0,1.0};
    m_color.getRgbF( (qreal*)&colord[0], (qreal*)&colord[1], (qreal*)&colord[2], (qreal*)&colord[3]);
    color[0] = colord[0];color[1] = colord[1];color[2] = colord[2];color[3] = colord[3];

    glGetError();
    glUniform4fv(attC,1,color );
    TEST_ERROR ("Attrib pointer color");




    //qDebug()<<QTime::currentTime().toString()<<"Drawing data "<<data;

    //levy dolni
    GLfloat x = GLfloat(m_itemOrigin.x());
    GLfloat zero = 0.0;
    if ( m_fmaximum < 0.0 ) zero = m_fmaximum;
    if ( m_fminimum > 0.0 ) zero = m_fminimum;
    GLfloat scale = GLfloat( m_itemSize.height() ) / (m_fmaximum-m_fminimum);
    GLfloat ymax = GLfloat(m_viewportSize.height()) - GLfloat(m_itemOrigin.y());
    GLfloat ymin = GLfloat(m_viewportSize.height()) - GLfloat(m_itemOrigin.y()) - GLfloat(m_itemSize.height()) ;
    GLfloat yzero = GLfloat(m_viewportSize.height()) - GLfloat(m_itemOrigin.y()) - scale * (m_fmaximum-zero) ;

    if ( m_iBytesPerSample == 1 )
    {
        unsigned char* ptr = (unsigned char*)&data[m_iOffset];
        for (int i=0, j=0;i<m_iLength;i+=2) {

            GLfloat x1 = x + GLfloat( i ) * GLfloat( m_itemSize.width() ) / m_iLength;
            m_arrCoords[j] = x1;j++;
            m_arrCoords[j] = yzero;j++;
            GLfloat y2add = GLfloat( *ptr - 0x80 ) / 0x7F * scale ;
            ptr += 2 * (m_iBytesPerSample + m_iStride);
            if ( y2add >= 0 && y2add < 1.0 ) {

                y2add = 1.0;
            }
            else if ( y2add < 0 && y2add > -1.0 ) {

                y2add = -1.0;
            }
            GLfloat y2 = yzero + y2add;
            if (y2 > ymax ) y2=ymax;
            if (y2 < ymin ) y2=ymin;
            m_arrCoords[j] = x1;j++;
            m_arrCoords[j] = y2;j++;
            GLfloat x2 = x + GLfloat(i + 1) * GLfloat( m_itemSize.width() ) / m_iLength;
            m_arrCoords[j] = x2;j++;
            m_arrCoords[j] = y2;j++;

            m_arrCoords[j] = x1;j++;
            m_arrCoords[j] = yzero;j++;

            m_arrCoords[j] = x2;j++;
            m_arrCoords[j] = y2;j++;

            m_arrCoords[j] = x2;j++;
            m_arrCoords[j] = yzero;j++;
        }
    }
    else if(m_iBytesPerSample==4)
    {
        unsigned char* ptr = &data[m_iOffset];
        for (int i=0, j=0;i<m_iLength;i+=2) {

            GLfloat x1 = x + GLfloat( i ) * GLfloat( m_itemSize.width() ) / m_iLength;
            m_arrCoords[j] = x1;j++;
            m_arrCoords[j] = yzero;j++;
            GLfloat y2add = GLfloat( *((float*)ptr) )  * scale;
            ptr += 2 * ( m_iBytesPerSample + m_iStride );
            if ( y2add >= 0 && y2add < 1.0 ) {

                y2add = 1.0;
            }
            else if ( y2add < 0 && y2add > -1.0 ) {

                y2add = -1.0;
            }
            GLfloat y2 = yzero + y2add;
            if (y2 > ymax ) y2=ymax;
            if (y2 < ymin ) y2=ymin;
            m_arrCoords[j] = x1;j++;
            m_arrCoords[j] = y2;j++;
            GLfloat x2 = x + GLfloat(i + 1) * GLfloat( m_itemSize.width() ) / m_iLength;
            m_arrCoords[j] = x2;j++;
            m_arrCoords[j] = y2;j++;

            m_arrCoords[j] = x1;j++;
            m_arrCoords[j] = yzero;j++;

            m_arrCoords[j] = x2;j++;
            m_arrCoords[j] = y2;j++;

            m_arrCoords[j] = x2;j++;
            m_arrCoords[j] = yzero;j++;
        }
    }
    else
    {
        unsigned char* ptr = &data[m_iOffset];
        for (int i=0, j=0;i<m_iLength;i+=2) {

            GLfloat x1 = x + GLfloat( i ) * GLfloat( m_itemSize.width() ) / m_iLength;
            m_arrCoords[j] = x1;j++;
            m_arrCoords[j] = yzero;j++;
            GLfloat y2add = GLfloat( *((short*)ptr) ) / 0x7FFF * scale ;
            ptr += 2 * (m_iBytesPerSample + m_iStride);
            if ( y2add >= 0 && y2add < 1.0 ) {

                y2add = 1.0;
            }
            else if ( y2add < 0 && y2add > -1.0 ) {

                y2add = -1.0;
            }
            GLfloat y2 = yzero + y2add;
            if (y2 > ymax ) y2=ymax;
            if (y2 < ymin ) y2=ymin;
            m_arrCoords[j] = x1;j++;
            m_arrCoords[j] = y2;j++;
            GLfloat x2 = x + GLfloat(i + 1) * GLfloat( m_itemSize.width() ) / m_iLength;
            m_arrCoords[j] = x2;j++;
            m_arrCoords[j] = y2;j++;

            m_arrCoords[j] = x1;j++;
            m_arrCoords[j] = yzero;j++;

            m_arrCoords[j] = x2;j++;
            m_arrCoords[j] = y2;j++;

            m_arrCoords[j] = x2;j++;
            m_arrCoords[j] = yzero;j++;
        }
    }


    glVertexAttribPointer( attVetices, 2 , GL_FLOAT , 0,0, m_arrCoords );
    TEST_ERROR ("Attrib pointer vertices");

    QMatrix4x4 matrix;
    matrix.setToIdentity();
    matrix.ortho(0, m_viewportSize.width() , 0, m_viewportSize.height(), -1, 1);
    glUniformMatrix4fv(attPM,1,false, matrix.constData());
    TEST_ERROR (" uniform pm");

//    if ( m_bClear ) {

//        glViewport(0,0,m_viewportSize.width(),m_viewportSize.height());
//        glClearColor(0, 0, 0, 1);
//        glClear(GL_COLOR_BUFFER_BIT);
//        // Calculate model view transformation
//        glMatrixMode( GL_MODELVIEW );                // zacatek modifikace projekcni matice

//        m_bClear = false;
//    }

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);


    int len = m_iLength / 2;
    if ( m_iLength & 1) len++;
    glDrawArrays(GL_TRIANGLES, 0, len * 6);


    if ( m_alternateColor == Qt::transparent )
    {
        glUniform4fv(attC,1,color );
        TEST_ERROR ("Attrib pointer alt color");
    }
    else {

        GLfloat color[] = {1.0,0.0,0.0,1.0};
        qreal colord[]= {0.0,0.0,0.0,1.0};
        m_alternateColor.getRgbF( (qreal*)&colord[0], (qreal*)&colord[1], (qreal*)&colord[2], (qreal*)&colord[3]);
        color[0] = colord[0];color[1] = colord[1];color[2] = colord[2];color[3] = colord[3];
        glUniform4fv(attC,1,color );
        TEST_ERROR ("Attrib pointer alt color 2 ");
    }

    if ( m_iBytesPerSample == 1 )
    {
        unsigned char* ptr = (unsigned char*)&data[m_iOffset];
        ptr += m_iBytesPerSample + m_iStride;
        for (int i=1, j=0;i<m_iLength;i+=2) {

            GLfloat x1 = x + GLfloat( i ) * GLfloat( m_itemSize.width() ) / m_iLength;
            m_arrCoords[j] = x1;j++;
            m_arrCoords[j] = yzero;j++;
            GLfloat y2add = GLfloat( *ptr - 0x80 ) / 0x7F * scale;
            ptr += 2 * (m_iBytesPerSample + m_iStride);
            if ( y2add >= 0 && y2add < 1.0 ) {

                y2add = 1.0;
            }
            else if ( y2add < 0 && y2add > -1.0 ) {

                y2add = -1.0;
            }
            GLfloat y2 = yzero + y2add;
            if (y2 > ymax ) y2=ymax;
            if (y2 < ymin ) y2=ymin;
            m_arrCoords[j] = x1;j++;
            m_arrCoords[j] = y2;j++;
            GLfloat x2 = x + GLfloat(i + 1) * GLfloat( m_itemSize.width() ) / m_iLength;
            m_arrCoords[j] = x2;j++;
            m_arrCoords[j] = y2;j++;

            m_arrCoords[j] = x1;j++;
            m_arrCoords[j] = yzero;j++;

            m_arrCoords[j] = x2;j++;
            m_arrCoords[j] = y2;j++;

            m_arrCoords[j] = x2;j++;
            m_arrCoords[j] = yzero;j++;
        }
    }
    else if(m_iBytesPerSample==4)
    {
        unsigned char* ptr = &data[m_iOffset];
        ptr += m_iBytesPerSample + m_iStride;
        for (int i=1, j=0;i<m_iLength;i+=2) {

            GLfloat x1 = x + GLfloat( i ) * GLfloat( m_itemSize.width() ) / m_iLength;
            m_arrCoords[j] = x1;j++;
            m_arrCoords[j] = yzero;j++;
            GLfloat y2add = GLfloat( *((float*)ptr) )  * scale ;
            ptr += 2 * ( m_iBytesPerSample + m_iStride );
            if ( y2add >= 0 && y2add < 1.0 ) {

                y2add = 1.0;
            }
            else if ( y2add < 0 && y2add > -1.0 ) {

                y2add = -1.0;
            }
            GLfloat y2 = yzero + y2add;
            if (y2 > ymax ) y2=ymax;
            if (y2 < ymin ) y2=ymin;
            m_arrCoords[j] = x1;j++;
            m_arrCoords[j] = y2;j++;
            GLfloat x2 = x + GLfloat(i + 1) * GLfloat( m_itemSize.width() ) / m_iLength;
            m_arrCoords[j] = x2;j++;
            m_arrCoords[j] = y2;j++;

            m_arrCoords[j] = x1;j++;
            m_arrCoords[j] = yzero;j++;

            m_arrCoords[j] = x2;j++;
            m_arrCoords[j] = y2;j++;

            m_arrCoords[j] = x2;j++;
            m_arrCoords[j] = yzero;j++;
        }
    }
    else
    {
        unsigned char* ptr = &data[m_iOffset];
        ptr += m_iBytesPerSample + m_iStride;
        for (int i=1, j=0;i<m_iLength;i+=2) {

            GLfloat x1 = x + GLfloat( i ) * GLfloat( m_itemSize.width() ) / m_iLength;
            m_arrCoords[j] = x1;j++;
            m_arrCoords[j] = yzero;j++;
            GLfloat y2add = GLfloat( *((short*)ptr) ) / 0x7FFF * scale ;
            ptr += 2 * (m_iBytesPerSample + m_iStride);
            if ( y2add >= 0 && y2add < 1.0 ) {

                y2add = 1.0;
            }
            else if ( y2add < 0 && y2add > -1.0 ) {

                y2add = -1.0;
            }
            GLfloat y2 = yzero + y2add;
            if (y2 > ymax ) y2=ymax;
            if (y2 < ymin ) y2=ymin;
            m_arrCoords[j] = x1;j++;
            m_arrCoords[j] = y2;j++;
            GLfloat x2 = x + GLfloat(i + 1) * GLfloat( m_itemSize.width() ) / m_iLength;
            m_arrCoords[j] = x2;j++;
            m_arrCoords[j] = y2;j++;

            m_arrCoords[j] = x1;j++;
            m_arrCoords[j] = yzero;j++;

            m_arrCoords[j] = x2;j++;
            m_arrCoords[j] = y2;j++;

            m_arrCoords[j] = x2;j++;
            m_arrCoords[j] = yzero;j++;
        }
    }


    glVertexAttribPointer( attVetices, 2 , GL_FLOAT , 0,0, m_arrCoords );
    TEST_ERROR ("Attrib pointer vertices");

    int len2 = m_iLength / 2;
    glDrawArrays(GL_TRIANGLES, 0, len2 * 6);


    glDisableVertexAttribArray(glGetAttribLocation(m_program, "vertices"));

    //qDebug()<<QTime::currentTime().toString()<<"Drawing data end "<<data;
    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
    //m_window->resetOpenGLState();
}


QVariant CBarGraphRenderer::getApplicationInitializeFailString()
{

        QString   versionMessage="";
        if( m_majorVersion != -1 )
        {

            if ( m_majorVersion<3 ) {

                versionMessage = "Detected openGl version " + QString::number( m_majorVersion ) + "." + QString::number( m_minorVersion ) + ". Please install openGL at least vesrion 3.0.";
            }
        }

        return QVariant( versionMessage );
}

