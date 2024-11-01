#include "writefile.h"

writeedafile::writeedafile(int ffnum,int ff_buffnum,int buff_buffnum,int fanout,readedafile *myfile,int buff_buff_cluster_num)
{
    myff_info=new ff_info[ffnum];
    my_ff_buff_info=new ff_buff_info[ff_buffnum];
    my_ff_buff_num=ff_buffnum;
    my_buff_buff_info=new buff_buff_info[buff_buff_cluster_num];
    my_buff_buff_num=buff_buff_cluster_num;
    max_fanout=fanout;
    UNITS_DISTANCE=myfile->myffdot.UNITS_DISTANCE;
    area=myfile->myffdot.area;
    my_ffsize=myfile->myffdot.my_ffsize;
    my_buffsize=myfile->myffdot.my_buffsize;
    clk_coordinate=myfile->myffdot.clk_coordinate;
    COMPONENTNUM=ffnum;
    COMPONENTNUM+=ff_buffnum;
    COMPONENTNUM+=buff_buff_cluster_num;
    NETSNUM=ff_buffnum+buff_buff_cluster_num+1;
    cout<<"NETSNUM"<<endl; 
    cout<<NETSNUM<<endl; 
    my_net_info= new net_info[(int)NETSNUM];
    my_ffnum=ffnum;
    my_buffnum=NETSNUM;
    mybuff_info=new buff_info[(int)my_buffnum]();
}
void writeedafile::get_ffinfo(int* labels,double* ffcoord,int num)
{
    for(int i=0;i<num;i++)
    {
        myff_info[i].clusternum=labels[i];
        myff_info[i].x=ffcoord[i*2];
        myff_info[i].y=ffcoord[i*2+1];
        myff_info[i].ffname=to_string(i+1);

    }


}
void writeedafile::get_buffinfo(int* labels,double* ffcoord,int num,ff_buff_info* info_buff)
{
    for(int i=0;i<num;i++)
    {
        mybuff_info[i].clusternum=labels[i];
        mybuff_info[i].x=ffcoord[i*2];
        mybuff_info[i].y=ffcoord[i*2+1];
        mybuff_info[i].buffname=info_buff[i].buffname;

    }


}
void writeedafile::get_ff_buff_info(ff_buff_info* info_buff,int cluster_num,int ffnum)
{
    for(int i=0;i<cluster_num;i++)
    {
        my_ff_buff_info[i].buffname=info_buff[i].buffname;
        my_ff_buff_info[i].connect_ff_num=0;
        my_ff_buff_info[i].front_clusternum=info_buff[i].front_clusternum;
        my_ff_buff_info[i].x=info_buff[i].x;
        my_ff_buff_info[i].y=info_buff[i].y;
        // my_net_info[i].name=to_string(writefile->netnamecount);
        // my_net_info[i].connect_comp
        // writefile->netnamecount++;


    }

    for(int i=0;i<ffnum;i++)
    {
        my_ff_buff_info[myff_info[i].clusternum].connect_ff.emplace_back(myff_info[i]);
        my_ff_buff_info[myff_info[i].clusternum].connect_ff_num++;
    }


}
void writeedafile::get_buff_buff_info(buff_buff_info* info_buff,int cluster_num,int ff_buff_num)
{
    for(int i=0;i<cluster_num;i++)
    {

        my_buff_buff_info[i].buffname=info_buff[i].buffname;
        my_buff_buff_info[i].connect_buff_num=0;
        my_buff_buff_info[i].front_clusternum=info_buff[i].front_clusternum;
        my_buff_buff_info[i].x=info_buff[i].x;
        my_buff_buff_info[i].y=info_buff[i].y;
        // my_net_info[i].name=to_string(writefile->netnamecount);
        // my_net_info[i].connect_comp
        // writefile->netnamecount++;


    }
    for(int i=0;i<ff_buff_num;i++)
    {
        my_buff_buff_info[mybuff_info[i].clusternum].connect_buff.emplace_back(mybuff_info[i]);
        my_buff_buff_info[mybuff_info[i].clusternum].connect_buff_num++;
        // cout<<mybuff_info[i].clusternum<<endl;
    }


}

void writeedafile::get_net_info(void)
{
    
    for(int i=0;i<my_ff_buff_num;i++)
    {
        my_net_info[i].name=to_string((int)netnamecount);
        netnamecount++;
        my_net_info[i].buffname=my_ff_buff_info[i].buffname;
        
        for(int j=0;j<my_ff_buff_info[i].connect_ff.size();j++)
        {
            
            
            my_net_info[i].connect_comp.emplace_back(my_ff_buff_info[i].connect_ff[j].ffname);
            
        }
    }  
    // for(int i=0;i<my_ff_buff_num;i++)
    // {
    //     for(int j=0;j<my_net_info[i].connect_comp.size();j++)
    //     {
    //         cout<<my_net_info[i].connect_comp[j]<<endl; 
    //     }
    // }
        
    
    cout<<"im ok2"<<endl; 
    for(int i=my_ff_buff_num;i<my_ff_buff_num+my_buff_buff_num;i++)
    {
        
        my_net_info[i].name=to_string((int)netnamecount);
        netnamecount++;
        my_net_info[i].buffname=my_buff_buff_info[i-my_ff_buff_num].buffname;
        
        for(int j=0;j<my_buff_buff_info[i-my_ff_buff_num].connect_buff.size();j++)
        {
            
            my_net_info[i].connect_comp.emplace_back(my_buff_buff_info[i-my_ff_buff_num].connect_buff[j].buffname);

        }

    }
    // for(int i=my_ff_buff_num;i<my_ff_buff_num+my_buff_buff_num;i++)
    // {
    //     // for(int j=0;j<my_net_info[i].connect_comp.size();j++)
    //     // {
    //     //     cout<<my_net_info[i].connect_comp[j]<<endl; 
    //     // }

    //      cout<<my_net_info[i].connect_comp.size()<<endl; 
    // }



}

void writeedafile::write(string name)
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
            write_solutiondef << "- FF" <<myff_info[i].ffname<<" "<<"FF "<<"( "<<myff_info[i].x<<" "<<myff_info[i].y<<" )"<<" "<<";"<<endl;
        }
        for(int i=0;i<my_ff_buff_num;i++)
        {
            write_solutiondef << "- BUF" <<my_ff_buff_info[i].buffname<<" "<<"BUF "<<"( "<<my_ff_buff_info[i].x<<" "<<my_ff_buff_info[i].y<<" )"<<" "<<";"<<endl;
        }
        for(int i=0;i<my_buff_buff_num;i++)
        {
            write_solutiondef << "- BUF" <<my_buff_buff_info[i].buffname<<" "<<"BUF "<<"( "<<my_buff_buff_info[i].x<<" "<<my_buff_buff_info[i].y<<" )"<<" "<<";"<<endl;
        }

        write_solutiondef << "END COMPONENTS ;" <<endl;
        write_solutiondef << "NETS " <<NETSNUM<<" ;"<<endl;
        for(int i=0;i<my_ff_buff_num;i++)
        {
            write_solutiondef << "- net_" <<i<<" "<<"( "<<"BUF"<<my_net_info[i].buffname<<" )"<<" ( ";

            for(int j=0;j<my_net_info[i].connect_comp.size();j++)
            {
                write_solutiondef << "FF" <<my_net_info[i].connect_comp[j]<<" ";
            }
            write_solutiondef << ") ;" <<endl;

        }
        for(int i=0;i<my_buff_buff_num;i++)
        {
            write_solutiondef << "- net_" <<i+my_ff_buff_num<<" "<<"( "<<"BUF"<<my_net_info[i+my_ff_buff_num].buffname<<" )"<<" ( ";

            for(int j=0;j<my_net_info[i+my_ff_buff_num].connect_comp.size();j++)
            {
                write_solutiondef << "BUF" <<my_net_info[i+my_ff_buff_num].connect_comp[j]<<" ";
            }
            write_solutiondef << ") ;" <<endl;

        }
        write_solutiondef << "- net_" <<my_buff_buff_num+my_ff_buff_num<<" "<<"( "<<"CLK"<<" )"<<" ( ";
        for(int i=my_ff_buff_num;i<my_ff_buff_num+my_buff_buff_num;i++)
        {
            write_solutiondef << "BUF" <<my_net_info[i].buffname<<" ";
        }
        write_solutiondef << ") ;" <<endl;
        write_solutiondef << "END NETS ;" <<endl;
        write_solutiondef.close();//关闭文件        
    }



}


