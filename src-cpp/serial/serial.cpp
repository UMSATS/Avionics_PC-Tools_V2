#include "serial.h"
#include <vector>
#include "c_serial.h"
#include <iostream>
#include <thread>

std::vector<std::string> serial::list_ports()
{
    std::vector<std::string> vectorOfPorts;
    const char** port_list = c_serial_get_serial_ports_list();

    int i = 0;
    while( port_list[ i ] != NULL )
    {
        const char * port = port_list [ i ];
        vectorOfPorts.push_back(port);
        i++;
    }

    c_serial_free_serial_ports_list( port_list );
    return vectorOfPorts;
}


serial::Serial::Serial ()
: mLines ( std::make_unique<c_serial_control_lines_t> ( ) )
{
    c_serial_set_global_log_function ( c_serial_stderr_log_function ) ;
    if ( c_serial_new ( &mPort, NULL ) < 0 )
    {
        fprintf( stderr, "ERROR: Unable to create new serial port\n" );
    }
}

serial::Serial::~Serial()
{
    if ( isOpen ( ) )
    {
        flush ( );
        close ( );
    }

    c_serial_free ( mPort );
}


void serial::Serial::setPortName(const std::string & portName)
{
    if ( c_serial_set_port_name ( mPort, portName.c_str() ) < 0 )
    {
        fprintf( stderr, "ERROR: can't set port name\n" );
    }
}

int serial::Serial::available ( )
{
    int available_bytes = 0;
    if ( c_serial_get_available( mPort, &available_bytes) < 0 )
    {
        fprintf( stderr, "ERROR: can't get available\n" );
    }

    return available_bytes;
}

bool serial::Serial::isOpen () const
{
    return c_serial_is_open ( mPort );
}

void serial::Serial::open ( )
{
    c_serial_open ( mPort );
}

void serial::Serial::close ( )
{
    c_serial_close ( mPort );
}

void serial::Serial::flush ( )
{
    c_serial_flush ( mPort );
}

bool serial::Serial::write ( char data )
{
    return write ( std::string (1, data ) );
}

bool serial::Serial::write ( uint8_t data )
{
    return write ( std::string (1, data ) );
}

bool serial::Serial::write ( const std::string & data )
{
    uint8_t * bytearray = reinterpret_cast < uint8_t* > ( const_cast < char* > ( data.c_str() ) );
    unsigned bytes_written = 0;
    if ( write ( bytearray, data.size(), &bytes_written ) != 0)
    {
        return false;
    }

    if (bytes_written != data.size() )
    {
        fprintf(stderr, "ERROR: can't write the whole message\n");
        return false;
    }

    return true;
}

bool serial::Serial::write ( uint8_t * data, unsigned length, unsigned * bytes_written )
{
    return c_serial_write_data ( mPort, data, length, bytes_written ) == 0;
}

int serial::Serial::readAll ( uint8_t * buffer )
{
//    uint8_t header [4];
//    unsigned bytes_read;
//    if ( c_serial_read_data_blocking ( mPort, &header, 4, &bytes_read ) != 0 )
//    {
//        fprintf( stderr, "ERROR: can't get the right header\n" );
//        return {};
//    }
//
//    if ( bytes_read != 4 )
//    {
//        fprintf( stderr, "ERROR: couldn't read the whole header!\n" );
//        return {};
//    }
//
//    // decoding the header
//    static union {
//        uint32_t value;
//        uint8_t bytes[4];
//    } u;
//
//    memcpy(u.bytes, header, 4);
//
//    // updating the length of the subsequent message
//    unsigned length = u.value;
//
//    std::cout << "Header: " << length << std::endl;

    unsigned bytes_read = 0;
    unsigned total_read = 0;

    while ( available() > 0 )
    {
        if ( c_serial_read_data (mPort, &buffer [total_read], 2048, &bytes_read, mLines.get() ) != 0) {
            fprintf(stderr, "ERROR: can't get the body\n");
            return {};
        };

        total_read += bytes_read;
    }



//    if ( bytes_read != length )
//    {
//        fprintf( stderr, "ERROR: couldn't read the whole message!\n" );
//        return {};
//    }

    return total_read;
}


void serial::Serial::setBaudRate ( baudrate_t baudRate )
{
    c_serial_set_baud_rate ( mPort, static_cast<CSerial_Baud_Rate> ( baudRate) );
}

void serial::Serial::setStopBits ( stopbits_t bits )
{
    c_serial_set_stop_bits ( mPort, static_cast<CSerial_Stop_Bits> ( bits ) );
}

void serial::Serial::setDataBits ( bytesize_t bits )
{
    c_serial_set_data_bits ( mPort, static_cast<CSerial_Data_Bits> ( bits ) );
}

void serial::Serial::setParity ( parity_t parity )
{
    c_serial_set_parity ( mPort, static_cast<CSerial_Parity> ( parity ) );
}

void serial::Serial::setFlowControl ( flowcontrol_t flow_control )
{
    c_serial_set_flow_control ( mPort, static_cast<CSerial_Flow_Control> ( flow_control ) );
}
