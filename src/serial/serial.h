#ifndef SERIAL_H
#define SERIAL_H

#include <memory>
#include <vector>

typedef struct c_serial_port c_serial_port_t;
typedef struct c_serial_control_lines c_serial_control_lines_t;

namespace serial
{
    /*!
    * Enumeration defines the possible bytesizes for the serial port.
    */
    typedef enum {
      fivebits = 5,
      sixbits = 6,
      sevenbits = 7,
      eightbits = 8
    } bytesize_t;

    /*!
     * Enumeration defines the possible parity types for the serial port.
     */
    typedef enum {
      parity_none = 0,
      parity_odd = 1,
      parity_even = 2,
      parity_mark = 3,
      parity_space = 4
    } parity_t;

    /*!
     * Enumeration defines the possible stopbit types for the serial port.
     */
    typedef enum {
      stopbits_one = 1,
      stopbits_two = 2,
      stopbits_one_point_five
    } stopbits_t;

    /*!
     * Enumeration defines the possible flowcontrol types for the serial port.
     */
    typedef enum {
      flowcontrol_none = 0,
      flowcontrol_software,
      flowcontrol_hardware
    } flowcontrol_t;


    /*!
    * Enumeration defines the possible flowcontrol types for the serial port.
    */
    typedef enum {
        /** Not supported by Windows */
        baudrate_0 = 0,

        /** Not supported by Windows */
        baudrate_50 = 50,

        /** Not supported by Windows */
        baudrate_100 = 100,

        baudrate_110 = 110,
        /** Not supported by Windows */
        baudrate_134 = 134,

        /** Not supported by Windows */
        baudrate_150 = 150,

        /** Not supported by Windows */
        baudrate_200 = 200,
        baudrate_300 = 300,
        baudrate_600 = 600,
        baudrate_1200 = 1200,

        /** Not supported by Windows */
        baudrate_1800 = 1800,

        baudrate_2400 = 2400,
        baudrate_4800 = 4800,
        baudrate_9600 = 9600,
        baudrate_19200 = 19200,
        baudrate_38400 = 38400,
        baudrate_115200 = 115200
    } baudrate_t;


    std::vector<std::string> list_ports();

    class Serial
    {
    public:
        Serial ();
        ~Serial();

        void setPortName(const std::string & portName);
        int available ( );

        bool isOpen () const;
        void open ( );
        void close ( );
        void flush ( );
        bool write ( char data );
        bool write ( uint8_t data );
        bool write ( const std::string & data );

        bool write ( uint8_t * data, unsigned length, unsigned * bytes_written );
        int readAll ( uint8_t * buffer );

        void setBaudRate ( baudrate_t baudRate );

        void setStopBits ( stopbits_t bits );
        void setDataBits ( bytesize_t bits );
        void setParity ( parity_t parity );

        void setFlowControl ( flowcontrol_t flow_control );

    private:
        c_serial_port* mPort;
        std::unique_ptr<c_serial_control_lines_t> mLines;
//        std::string mBuffer;
    };
}


#endif // SERIAL_H