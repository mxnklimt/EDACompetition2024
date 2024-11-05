#include "writefile.h"

writeedafile::writeedafile(int ffnum,int buffnum,int fanout,readedafile *myfile)
{
    max_fanout=fanout;
    UNITS_DISTANCE=myfile->myffdot.UNITS_DISTANCE;
    area=myfile->myffdot.area;
    my_ffsize=myfile->myffdot.my_ffsize;
    my_buffsize=myfile->myffdot.my_buffsize;
    clk_coordinate=myfile->myffdot.clk_coordinate;
    COMPONENTNUM=ffnum;
    COMPONENTNUM+=buffnum;
    NETSNUM=buffnum+1;
    my_ffnum=ffnum;
    my_buffnum=buffnum;

}


void writeedafile::write(string name,std::vector<std::vector<Point>> &clusters_data,double* ffdata,vector<int> &buff_cluster_num)
{
    my_solutiondef=name;
    write_solutiondef.open(my_solutiondef,ios::out);
    if (!write_solutiondef.is_open())//文件打开失败返回false
	{
		cout << "solutiondef创建失败" << endl;
	}else
    {

        write_solutiondef << "UNITS DISTANCE MICRONS " <<(int)UNITS_DISTANCE<<" ;"<<endl;
        write_solutiondef << "DIEAREA " <<"( "<<area.area1.x<<" "<<area.area1.y<<" )"<<" "
        <<"( "<<area.area2.x<<" "<<area.area2.y<<" )"<<" "
        <<"( "<<area.area3.x<<" "<<area.area3.y<<" )"<<" "
        <<"( "<<area.area4.x<<" "<<area.area4.y<<" )"<<" "
        <<";"<<endl;
        write_solutiondef << "FF " <<"( "<<my_ffsize.x<<" "<<my_ffsize.y<<" )"<<" "<<";"<<endl;
        write_solutiondef << "BUF " <<"( "<<my_buffsize.x<<" "<<my_buffsize.y<<" )"<<" "<<";"<<endl;
        write_solutiondef << "CLK " <<"( "<<clk_coordinate.x<<" "<<clk_coordinate.y<<" )"<<" "<<";"<<endl;
        write_solutiondef << "COMPONENTS " <<COMPONENTNUM<<" "<<";"<<endl;
        for(int i=0;i<my_ffnum;i++)
        {
            write_solutiondef << "- FF" <<i+1<<" "<<"FF "<<"( "<<(int)ffdata[i*2]<<" "<<(int)ffdata[i*2+1]<<" )"<<" "<<";"<<endl;
        }
        for(int i=0;i<my_buffnum;i++)
        {
            write_solutiondef << "- BUF" <<clusters_data[i][clusters_data[i].size()-1].name<<" "<<"BUF "<<"( "<<(int)clusters_data[i][clusters_data[i].size()-1].x<<" "<<(int)clusters_data[i][clusters_data[i].size()-1].y<<" )"<<" "<<";"<<endl;
        }


        write_solutiondef << "END COMPONENTS ;" <<endl;
        write_solutiondef << "NETS " <<NETSNUM<<" ;"<<endl;
        for(int i=0;i<my_buffnum;i++)
        {
            write_solutiondef << "- net_" <<i<<" "<<"( "<<"BUF"<<clusters_data[i][clusters_data[i].size()-1].name<<" )"<<" ( ";

            for(int j=0;j<clusters_data[i].size()-1;j++)
            {
                if(clusters_data[i][j].type==0)
                {
                    write_solutiondef << "FF" <<clusters_data[i][j].name<<" ";
                }
                if(clusters_data[i][j].type==1)
                {
                    write_solutiondef << "BUF" <<clusters_data[i][j].name<<" ";
                }
                
            }
            write_solutiondef << ") ;" <<endl;

        }

        write_solutiondef << "- net_" <<my_buffnum<<" "<<"( "<<"CLK"<<" )"<<" ( ";
        for(int i=my_buffnum-*(++buff_cluster_num.rbegin());i<my_buffnum;i++)
        {
            write_solutiondef << "BUF" <<clusters_data[i][clusters_data[i].size()-1].name<<" ";
        }
        write_solutiondef << ") ;" <<endl;
        write_solutiondef << "END NETS ;" <<endl;
        write_solutiondef.close();//关闭文件        
    }



}


