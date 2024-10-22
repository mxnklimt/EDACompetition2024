//#define _CRT_SECURE_NO_WARNINGS
#include "readfile.h"
#include "ffdot.h"
void readedafile::setfilename(string constraintstxt, string problemdef)
{
	my_constraintstxt = constraintstxt;
	my_problemdef = problemdef;
	cout << my_constraintstxt << endl;
	cout << my_problemdef << endl;
}
int readedafile::get_constraintstxt(void)
{
	cout << "read  " << my_constraintstxt.c_str() << endl;
	read_constraintstxt.open(my_constraintstxt.c_str());
	if (read_constraintstxt.is_open())
	{
		string line;
		while (getline(read_constraintstxt, line))
		{
			cout << line << endl;
			char buff[40];
			double num;
			sscanf(line.c_str(), "%s = %lf", buff, &num);
			cout << num << endl;
			if (strcmp(buff, "net_unit_r") == 0)
			{
				net_unit_r = num;
			}
			else if (strcmp(buff, "net_unit_c") == 0)
			{
				net_unit_c = num;
			}
			else if (strcmp(buff, "max_net_rc") == 0)
			{
				max_net_rc = num;
			}
			else if (strcmp(buff, "max_fanout") == 0)
			{
				max_fanout = num;
			}
			else if (strcmp(buff, "buffer_delay") == 0)
			{
				buffer_delay = num;
			}
			cout << net_unit_r << "\n" << net_unit_c << "\n" << max_net_rc << "\n" << max_fanout << "\n" << buffer_delay << "\n" << endl;
		}
	}
	else
	{
		cout << "open err" << endl;
	}
	read_constraintstxt.close();

	return 1;
}

int readedafile::get_ffdot(void)
{
	cout << "read  " << my_problemdef.c_str() << endl;
	read_problemdef.open(my_problemdef.c_str());
	int ffsizefindok = 0;
	int numfindok = 0;
	if (read_problemdef.is_open())
	{
		string line;
		string strbuff;
		char buff[40];
		int point = 0;
		int strnumbuff;
		int ffpoint = 0;
		while (getline(read_problemdef, line))
		{

			if (line.find("UNITS DISTANCE MICRONS ") != string::npos)
			{
				point = line.find("UNITS DISTANCE MICRONS ");
				strbuff = line.substr(point + sizeof("UNITS DISTANCE MICRONS ") - 1, line.length() - point);
				cout << strbuff << endl;
				sscanf(strbuff.c_str(), "%d", &strnumbuff);
				myffdot.UNITS_DISTANCE = strnumbuff;
				cout << myffdot.UNITS_DISTANCE << endl;

			}
			else if (line.find("DIEAREA ") != string::npos)
			{
				DIEAREA areabuff;
				point = line.find("DIEAREA ");
				strbuff = line.substr(point + sizeof("DIEAREA ") - 1, line.length() - point);
				cout << strbuff << endl;
				sscanf(strbuff.c_str(), "( %d %d ) ( %d %d ) ( %d %d ) ( %d %d )", &areabuff.area1.x, &areabuff.area1.y
					, &areabuff.area2.x, &areabuff.area2.y
					, &areabuff.area3.x, &areabuff.area3.y
					, &areabuff.area4.x, &areabuff.area4.y
				);

				myffdot.area = areabuff;
				cout << myffdot.area.area1.x << endl;
				cout << myffdot.area.area1.y << endl;
				cout << myffdot.area.area4.x << endl;
				cout << myffdot.area.area4.y << endl;

			}
			else if ((line.find("FF ") != string::npos) && (ffsizefindok == 0))
			{
				comp_size sizebuff;
				point = line.find("FF ");
				strbuff = line.substr(point + sizeof("FF ") - 1, line.length() - point);
				sscanf(strbuff.c_str(), "( %d %d ) ;", &sizebuff.x, &sizebuff.y);
				ffsizefindok = 1;
				myffdot.my_ffsize = sizebuff;
				cout << myffdot.my_ffsize.x << endl;
				cout << myffdot.my_ffsize.y << endl;


			}
			else if ((line.find("BUF ") != string::npos))
			{
				comp_size sizebuff;
				point = line.find("BUF ");
				strbuff = line.substr(point + sizeof("BUF ") - 1, line.length() - point);
				sscanf(strbuff.c_str(), "( %d %d ) ;", &sizebuff.x, &sizebuff.y);
				myffdot.my_buffsize = sizebuff;
				cout << myffdot.my_buffsize.x << endl;
				cout << myffdot.my_buffsize.y << endl;
			}
			else if ((line.find("CLK ") != string::npos))
			{
				two_coordinates buff;
				point = line.find("CLK ");
				strbuff = line.substr(point + sizeof("CLK ") - 1, line.length() - point);
				sscanf(strbuff.c_str(), "( %d %d ) ;", &buff.x, &buff.y);
				myffdot.clk_coordinate = buff;
				cout << myffdot.clk_coordinate.x << endl;
				cout << myffdot.clk_coordinate.y << endl;
			}
			else if ((line.find("COMPONENTS ") != string::npos) && (numfindok == 0))
			{
				double numbuff;
				point = line.find("COMPONENTS ");
				strbuff = line.substr(point + sizeof("COMPONENTS ") - 1, line.length() - point);
				sscanf(strbuff.c_str(), "%lf", &numbuff);
				myffdot.numofdot = numbuff;
				numfindok = 1;
				myffdot.data = new double[(int)myffdot.numofdot * 2];
				cout << myffdot.numofdot << endl;
			}
			else if (line.find("FF ", 4) != string::npos)
			{
				two_coordinates buff;
				point = line.find("FF ", 4);
				strbuff = line.substr(point + sizeof("FF ") - 1, line.length() - point);
				sscanf(strbuff.c_str(), "( %d %d )", &buff.x, &buff.y);

				myffdot.data[ffpoint * 2] = buff.x;

				myffdot.data[ffpoint * 2 + 1] = buff.y;

				ffpoint++;

				//cout<<myffdot.my_ffcell->x<<endl;
				//cout<<myffdot.my_ffcell->y<<endl;

			}
		}

	}
	else
	{
		cout << "open err" << endl;
	}
	read_problemdef.close();

	return 1;
}