/* Dummy stdio.h file that is necessary to have the functions documented in the Reference section */
/* of the File System Component */

/// \brief  Close file stream.
/// \param  stream File pointer specifying the data stream.
/// \return status information:
///         - 0 if the stream was successfully closed.
///         - nonzero if errors were detected or if stream was already closed.
extern int fclose(FILE * stream);

/// \brief  Flush file stream.
/// \param  stream File pointer specifying the data stream.
/// \return status information:
///         - nonzero if write error has occurred.
extern int fflush(FILE * stream);

/// \brief  Open file stream.
/// \param  filename File pointer specifying the file to open.
/// \param  mode Pointer defining the access type.
/// \return a pointer to object controlling stream. If the open operation fails,
///         fopen returns a null pointer.
extern FILE *fopen(const char * filename, const char * mode);


/// \brief  Reopen file stream.
/// \param  filename File pointer specifying the file to open.
/// \param  mode Pointer defining the access type.
/// \param  stream File pointer specifying the data stream.
/// \return a pointer to object controlling stream. If the open operation fails,
///         fopen returns a null pointer.
extern FILE *freopen(const char * filename, const char * mode, FILE * stream);

					
/// \brief  Buffer stream.
/// \param  stream File pointer specifying the data stream.
/// \param  buf Array pointer specifying the buffer for the stream.
extern void setbuf(FILE * stream, char * buf);
					
					
/// \brief  Buffer stream.
/// \param  stream File pointer specifying the data stream.
/// \param  buf Array pointer specifying the buffer for the stream.
/// \param  mode Specifies how th stream will be buffered.
/// \param  size Specifies the size of the array.
/// \return status information:
///         - 0 on success.
///         - nonzero if invalid value was given for mode or size, or if the
///           request can not be honoured.
extern int setvbuf(FILE * stream, char * buf, int mode, size_t size);

/// \brief  Write formatted string to file stream.				   
/// \param  stream File pointer specifying the data stream.
/// \param  format Pointer specifying the output format.
/// \return status information:
///         - Number of characters transmitted.
///         - Negative value if an output error occurred.
extern int fprintf(FILE * stream, const char * format, ...);


/// \brief  Read formatted string from file stream.
/// \param  stream File pointer specifying the data stream.
/// \param  format Pointer specifying the formatting string.
/// \return number of successfully converted input fields.
extern int fscanf(FILE * stream, const char * format, ...);
					
					
/// \param  stream File pointer specifying the data stream.
/// \param  format Pointer specifying the formatting string.
/// \param  arg A value identifying a variable argument list.
extern int vfscanf(FILE * stream, const char * format, va_list arg);


/// \param  stream File pointer specifying the data stream.
/// \param  format Pointer specifying the output format.
/// \param  arg A value identifying a variable argument list.
extern int vfprintf(FILE * stream, const char * format, va_list arg);
					
					
/// \brief Read character from file stream.					 
/// \param  stream File pointer specifying the data stream.
/// \return status information:
///         - Integer: representing the character.
///         - EOF: on error or end-of-file condition.
extern int fgetc(FILE * stream);

/// \brief Read string from file stream.
/// \param  s Pointer specifying the storage buffer.
/// \param  n Specifies the number of characters to read.
/// \param  stream File pointer specifying the data stream.
/// \return status information:
///         - \a s if successful.
///         - NULL: If end-of-file is encountered or a read error occurs and no characters.
extern char *fgets(char * s, int n, FILE * stream);

/// \brief Write character to file stream.
/// \param  c Integer value specifying the character to write.
/// \param  stream File pointer specifying the data stream.
/// \return status information:
///         - the character written if successful.
///         - EOF: on error.
extern int fputc(int c, FILE * stream);

/// \brief Write string to file stream.
/// \param  s Constant character pointer specifying the string to write.
/// \param  stream File pointer specifying the data stream.
/// \return status information:
///         - non-negative number if successful.
///         - EOF: on error.
extern int fputs(const char * s, FILE * stream);

/// \brief Read character from file stream (unsafe).					 
/// \param  stream File pointer specifying the data stream.
/// \return status information:
///         - the next character from the input stream pointed to by stream.
///         - EOF: If end-of-file is encountered or a read error occurs and no characters.
extern int getc(FILE * stream);


/// \brief Write character to file stream (unsafe).
/// \param  c Constant character pointer specifying the string to write.
/// \param  stream File pointer specifying the data stream.
/// \return status information:
///         - the character written if successful.
///         - EOF: on error.
extern int putc(int c, FILE * stream);


/// \brief Stores a character into an input file stream.
/// \param  c Constant character pointer specifying the string to write.
/// \param  stream File pointer specifying the data stream.
/// \return status information:
///         - the character pushed back after conversion.
///         - EOF if the operation fails.
extern int ungetc(int c, FILE * stream);

/// \brief Read number of bytes from file stream.
/// \param  ptr Void pointer specifying the storage buffer.
/// \param  size Defines the item size in bytes.
/// \param  nmemb Defines the number of items to read.
/// \param  stream File pointer specifying the data stream.
/// \return the number of members successfully read, which may be less than
///         \a nmemb if a read error or end-of-file is encountered. If \a size or
///         \a nmemb is zero, \b fread returns zero and the contents of the array
///         and the state of the stream remain unchanged.
extern size_t fread(void * ptr, size_t size, size_t nmemb, FILE * stream));
					
/// \brief Write number of bytes to file stream.					
/// \param  ptr Void pointer specifying the storage buffer.
/// \param  size Defines the item size in bytes.
/// \param  nmemb Defines the number of items to write.
/// \param  stream File pointer specifying the data stream.
/// \return the number of members successfully written, which will be less than
///         \a nmemb only if a write error is encountered.
extern size_t fwrite(const void * ptr, size_t size, size_t nmemb, FILE * stream));
					
					
/// \brief Store current value of file position indicator.					
/// \param  stream File pointer specifying the data stream.
/// \param  pos Pointer specifying the current value of the file position indicator.
/// \return status information:
///         - 0 if successful.
///         - Otherwise nonzero is returned and the integer
///           expression errno is set to an implementation-defined nonzero value.
extern int fgetpos(FILE * stream, fpos_t * pos);

/// \brief Move file stream's in-file pointer to new location.
/// \param  stream File pointer specifying the data stream.
/// \param  offset File Long integer value specifying the number of bytes to move.
/// \param  whence Integer defining the cursor location.
/// \return status information:
///         - 0 on success.
///         - EOF on error.
extern int fseek(FILE * stream, long int offset, int whence);


/// \brief Set file position indicator.					
/// \param  stream File pointer specifying the data stream.
/// \param  pos Pointer specifying the value of the file position indicator that is to be set.
/// \return status information:
///         - 0 if successful.
///         - Otherwise nonzero is returned and the integer
///           expression errno is set to an implementation-defined nonzero value.
extern int fsetpos(FILE * stream, const fpos_t * pos);

/// \brief Get current location of stream's in-file pointer.
/// \param  stream File pointer specifying the data stream.
/// \return status information:
///         - file cursor position if successful.
///         - -1L on error.
extern long int ftell(FILE * stream);

/// \brief Move file stream's in-file pointer to beginning of file.
/// \param  stream File pointer specifying the data stream.
extern void rewind(FILE * stream);


/// \brief Clear end-of-file and error indicators.
/// \param  stream File pointer specifying the data stream.
extern void clearerr(FILE * stream);

/// \brief Report whether end of stream has been reached.
/// \param  stream File pointer specifying the data stream.
/// \return status information:
///         - 0 if data stream can be read.
///         - non-zero value if end-of-file condition occurred.
extern int feof(FILE * stream);

/// \brief Report whether there is an error in file stream.
/// \param  stream File pointer specifying the data stream.
/// \return status information:
///         - 0 if there are no errors.
///         - non-zero value if errors occurred.
extern int ferror(FILE * stream);
/* end of dummy stdio.h */
