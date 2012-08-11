#ifndef Q_DEBUG_STREAM_H
#define Q_DEBUG_STREAM_H

#include <iostream>
#include <streambuf>
#include <string>

//This header file is taken from : http://lists.trolltech.com/qt-interest/2005-06/thread00166-0.html
//However it wrote to QTextEdit widget, I have (heavily) adapted it to write to a file.

class QDebugStream : public std::basic_streambuf<char>
{
public:
    QDebugStream(std::ostream &stream, char * filename) : m_stream(stream)
    {
        file = fopen(filename,"a+"); //a+ r/w append
        m_old_buf = stream.rdbuf();
        stream.rdbuf(this);
    }

    ~QDebugStream(){
        // output anything that is left
        if (!m_string.empty()){
            fprintf(file, m_string.c_str());
        }
        fprintf(file, "Hello there");
        fclose(file);
        m_stream.rdbuf(m_old_buf);
    }

protected:
    virtual int_type overflow(int_type v)
    {
//        if (v == '\n')
//        {
            fprintf(file, m_string.c_str());
//            m_string.erase(m_string.begin(), m_string.end());
//        }
//        else m_string += v;
        return v;
    }

    virtual std::streamsize xsputn(const char *p, std::streamsize n)
    {
        m_string.append(p, p + n);

        int pos = 0;
        while (pos != std::string::npos)
        {
            pos = m_string.find('\n');
            if (pos != std::string::npos)
            {
                std::string tmp(m_string.begin(), m_string.begin() + pos);
                fprintf(file, tmp.c_str());
                m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
            }
        }

        return n;
    }

private:
    std::ostream &m_stream;
    std::streambuf *m_old_buf;
    std::string m_string;
    FILE *file;
};

#endif
