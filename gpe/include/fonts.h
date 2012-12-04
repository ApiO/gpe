#ifndef fonts_h
#define fonts_h

/*
 * implementation de http://www.gamedev.net/topic/330742-quick-tutorial-variable-width-bitmap-fonts/
 */

 //CharDescriptor holds the information for a single character
typedef struct CharDescriptor
{
	int x, y;
	int Width, Height;
	int XOffset, YOffset;
	int XAdvance;
	int Page;
  int Chnl;
} CharDescriptor;

//Charset holds the descriptions that apply to all characters, as well as the descriptors for every character
typedef struct Charset
{
	int LineHeight;
	int Base;
	int Width, Height;
	int Pages;
	CharDescriptor Chars[256];
} Charset;

void fonts_foo_load(void);
void fonts_foo_draw(void);
void fonts_foo_free(void);


/*
bool ParseFont( std::istream& Stream, Font::Charset& CharsetDesc )
{
	String Line;
	String Read, Key, Value;
	std::size_t i;
	while( !Stream.eof() )
	{
		std::stringstream LineStream;
		std::getline( Stream, Line );
		LineStream << Line;

		//read the line's type
		LineStream >> Read;
		if( Read == "common" )
		{
			//this holds common data
			while( !LineStream.eof() )
			{
				std::stringstream Converter;
				LineStream >> Read;
				i = Read.find( '=' );
				Key = Read.substr( 0, i );
				Value = Read.substr( i + 1 );

				//assign the correct value
				Converter << Value;
				if( Key == "lineHeight" )
					Converter >> CharsetDesc.LineHeight;
				else if( Key == "base" )
					Converter >> CharsetDesc.Base;
				else if( Key == "scaleW" )
					Converter >> CharsetDesc.Width;
				else if( Key == "scaleH" )
					Converter >> CharsetDesc.Height;
				else if( Key == "pages" )
					Converter >> CharsetDesc.Pages;
			}
		}
		else if( Read == "char" )
		{
			//this is data for a specific char
			unsigned short CharID = 0;

			while( !LineStream.eof() )
			{
				std::stringstream Converter;
				LineStream >> Read;
				i = Read.find( '=' );
				Key = Read.substr( 0, i );
				Value = Read.substr( i + 1 );

				//assign the correct value
				Converter << Value;
				if( Key == "id" )
					Converter >> CharID;
				else if( Key == "x" )
					Converter >> CharsetDesc.Chars[CharID].x;
				else if( Key == "y" )
					Converter >> CharsetDesc.Chars[CharID].y;
				else if( Key == "width" )
					Converter >> CharsetDesc.Chars[CharID].Width;
				else if( Key == "height" )
					Converter >> CharsetDesc.Chars[CharID].Height;
				else if( Key == "xoffset" )
					Converter >> CharsetDesc.Chars[CharID].XOffset;
				else if( Key == "yoffset" )
					Converter >> CharsetDesc.Chars[CharID].YOffset;
				else if( Key == "xadvance" )
					Converter >> CharsetDesc.Chars[CharID].XAdvance;
				else if( Key == "page" )
					Converter >> CharsetDesc.Chars[CharID].Page;
				else if( Key == "chnl" )
					Converter >> CharsetDesc.Chars[CharID].Chanel;
			}
		}
	}

	return true;
}
*/

void fonts_bitmap	(char * text, int x, int y);
void fonts_outline	(char * text);
void fonts_texture_mapped_outline	(char * text);
void fonts_2d_texture	(char * text);

#endif //fonts_h