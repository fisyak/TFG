import os
import sys
import math
import datetime
import commands
from operator import truediv
import DBFun
#-----------------------------------------------------------------------------------
#     Developed by : Amol 
#     This python script parses the log file
#     Parse(filename)               --> Parse log file
#     ForEachFile(filename)   --> Check for log file if the job is completed successfully
#     ParseErrorFile(filename)      --> Parsing the error (not completed)
#     InsertDBRecords(parsing_info) --> Insert the parsing information into database
#-----------------------------------------------------------------------------------


def Parse(filename):
    row = {}
    #--/star/rcf/test/dev/daq_sl302.ittf/Fri/year_2014/production_15GeV_2014/xyz.log--
    row["jobID"]= "NewFrame"
    mtime = os.path.getmtime(filename)
    createtime=datetime.datetime.fromtimestamp(mtime).strftime('%Y-%m-%d %H:%M:%S')
    row["createTime"] = createtime
    row["crashedCode"] = "none"
    ppyear=filename.split("year_")[1].split("/")[0]
    if ppyear.find(".")>=0:
       ppyear = ppyear.split(".")[0]
    row["prodyear"]=ppyear
    jobstatus = "Done"
    error_message = "none"
    path = filename
    while path[len(path)-1]!= "/":
          path = path[:-1] #--removeing last character of string--
    row["path"]=path[:-1]
    row["jobStatus"]=jobstatus
    row["errMessage"]=error_message
    folders = filename.split("/")
    logfilename = folders[len(folders)-1]
    row["logFile"]=logfilename
    #--activated flag if embed contains in filename--
    embedflag = 0
    if filename.find("embed")>0:
       embedflag = 1
    #--QAInfo:You are using STAR_LEVEL : dev, ROOT_LEVEL : 5.34.30 and node : rcas6230.rcf.bnl.gov--
    cmd = "cat "+filename+" | grep \"QAInfo:You are using STAR_LEVEL\""
    output = commands.getoutput(cmd)
    #print(output)
    row["LibLevel"]=output.split("STAR_LEVEL : ")[1].split()[0][:-1]
    row["rootLevel"]=output.split("ROOT_LEVEL : ")[1].split()[0]
    row["nodeID"]=output.split("node : ")[1].split()[0]
    #-- For BTOF --
    cmd = "cat "+filename+" | grep '(matched)'"
    output = commands.getoutput(cmd)
    if len(output)>1:
       lines = output.split("\n")
       btofcount = 0
       totalbtof = 0
       for line in lines:
           btofcount = btofcount + 1
           lineparts = line.split()
           totalbtof = totalbtof + int(lineparts[10])
       avg_btof = truediv(totalbtof,btofcount)
       row["avg_no_btof"]=avg_btof
    else:
       row["avg_no_btof"]=0 
    #--QA :INFO  - ============= You are in DEV ===============--
    cmd = "cat "+filename+" | grep \"You are in\""
    output = commands.getoutput(cmd)
    parts = output.split(" ")
    liblevel = parts[8] #--DEV--
    row["LibTag"]=liblevel
    cmd = "cat "+filename+" | grep 'is loaded' | head -n 1"
    output = commands.getoutput(cmd)
    suboutput = output.split(row.get("LibTag"))
    subparts = suboutput[1].split("/")
    #print(subparts)
    row["gcc_version"]=subparts[1]
    if subparts[2]=='lib':
       row["optimized"]='No'
    else:
       row["optimized"]='Yes'
    #---- get the chain option, rank, primary tracks and vertex--
    vrank_no = 0
    no_event_vtx = 0
    primary_track = 0
    primary_track_nfit15 = 0
    if embedflag == 1:
       cmd = "cat "+filename+" | grep \"StRoot/macros/embedding\""
       #--Processing /afs/rhic.bnl.gov/star/packages/DEV/StRoot/macros/embedding/bfcMixer_TpcSvtSsd2005.C...
       output = commands.getoutput(cmd)
       parts = output.split("/")
       chain = parts[9] #--bfcMixer_TpcSvtSsd2005.C...--
       #--vertex rank --
       #cmd = "cat "+filename+" | grep \"QA :INFO\" | grep \"Rank\" | grep \"V\[  0\]\""
       cmd = "cat "+filename+" | grep -A2 \"V\[  0\]\""
       output = commands.getoutput(cmd)
       parts = output.split("\n")
       no_event_vtx = 0
       if len(parts) > 1:
           i = 0;
           while i < len(parts):
               line = parts[i]
               if line.find("#V[  0]")>0:
                  no_event_vtx = no_event_vtx + 1
                  newline = line.split(":")
                  newparts = newline[4].split("U/T/G")
                  if float(newparts[0])== -5:
                     vrank_no = vrank_no + 1
                     #--primary tracks--
                     if parts[i+1].find("primary all")>0:
                        primary_all_line = parts[i+1]
                        primary_track += int(primary_all_line.split(" ")[5].split("\'")[2])
                     if parts[i+2].find("primary good")>0:
                        primary_good_line = parts[i+2]
                        primary_track_nfit15 += int(primary_good_line.split(" ")[5].split("\'")[2])
               i = i + 1 
    else:
       cmd = "cat "+filename+" | grep \"Processing bfc.C\""
       output = commands.getoutput(cmd)
       parts = output.split("\"")
       chain = parts[1]  #--P2014a,btof,mtd,BEmcChkStat,CorrX,OSpaceZ2,OGridLeak3D,-hitfilt--      
       #--vertex rank --  
       #cmd = "cat "+filename+" | grep \"QA :INFO\" | grep \"Rank\" | grep \"V\[  0\]\""
       cmd = "cat "+filename+" | grep -A2 \"V\[  0\]\""
       output = commands.getoutput(cmd)
       parts = output.split("\n")
       no_event_vtx = 0
       if len(parts) > 1:
           i = 0;
           while i < len(parts):
               line = parts[i]
               if line.find("#V[  0]")>0:
                  no_event_vtx = no_event_vtx + 1
                  newline = line.split(":")
                  newparts = newline[4].split("U/T/")
                  #print(newparts[0]+"=>"+newline[4])
                  if float(newparts[0])>0.000:
                     vrank_no = vrank_no + 1
                     #--primary tracks--
                     if parts[i+1].find("primary all")>0:
                        primary_all_line = parts[i+1]
  			#print(primary_all_line)
                        primary_track += int(primary_all_line.split(" ")[5].split("\'")[2])
                     if parts[i+2].find("primary good")>0:
                        primary_good_line = parts[i+2]
    			#print(primary_good_line)
                        primary_track_nfit15 += int(primary_good_line.split(" ")[5].split("\'")[2])
               i = i + 1 
    row["chainOpt"]=chain
    row["NoEventVtx"]=no_event_vtx
    if no_event_vtx > 0:
       #print("Primary Track:"+str(primary_track)+" no_event_vtx="+str(no_event_vtx))
       row["avgNoVtx_evt"]= truediv(primary_track,no_event_vtx)
    else: 
       row["avgNoVtx_evt"]=0
    #--for QA jobs--
    mtdhits = 0
    pixelhits = 0
    isthits = 0
    ssthits = 0
    usedpixelhits = 0
    usedisthits = 0
    usedssthits = 0
    qadatasets = ["AuAu200_production_low_2014","AuAu200_production_mid_2014","production_pp200long_2015","production_pAu200_2015","AuAu200_production_2016"]
    for dataset in qadatasets:
        if filename.find(dataset)>0:
           #--StMtdHitMaker:INFO & MTD hits in event--
           cmd = "cat "+filename+" | grep \"StMtdHitMaker:INFO\" | grep \"MTD hits in event\" | awk \'{print $3}\'"
           output = commands.getoutput(cmd)
           parts = output.split("\n")
           if len(parts)>1:
              for no in parts:
                  mtdhits = mtdhits + int(no)
           #--PixelHitLoader loaded--
           cmd = "cat "+filename+" | grep \"PixelHitLoader loaded\" | awk \'{print $7}\'"
           output = commands.getoutput(cmd)
           parts = output.split("\n")
           if len(parts)>1:
              for no in parts:
                  pixelhits = pixelhits + int(no)
           #--IstHitLoader loaded--
           cmd = "cat "+filename+" | grep \"IstHitLoader loaded\" | awk \'{print $7}\'"
           output = commands.getoutput(cmd)
           parts = output.split("\n")
           if len(parts)>1:
              for no in parts:
                  isthits = isthits + int(no)
           #--SstHitLoader loaded-- 
           cmd = "cat "+filename+" | grep \"SstHitLoader loaded\" | awk \'{print $7}\'"
           output = commands.getoutput(cmd)
           parts = output.split("\n")
           if len(parts)>1:
              for no in parts:
                  ssthits = ssthits + int(no)
           #--Number of used hits:PxlId--
     	   cmd = "cat "+filename+" | grep \"Number of used hits:PxlId\" | awk \'{print $7}\'"
           output = commands.getoutput(cmd)
           parts = output.split("\n")
           if len(parts)>1:
              for no in parts:
                  usedpixelhits = usedpixelhits + int(no[1:])
           #--Number of used hits:IstId--
           cmd = "cat "+filename+" | grep \"Number of used hits:IstId\" | awk \'{print $7}\'"
           output = commands.getoutput(cmd)
           parts = output.split("\n")
           if len(parts)>1:
              for no in parts:
                  usedisthits = usedisthits + int(no[1:])
           #--Number of used hits:SstId--
           cmd = "cat "+filename+" | grep \"Number of used hits:SstId\" | awk \'{print $7}\'"
           output = commands.getoutput(cmd)
           parts = output.split("\n")
           if len(parts)>1:
              for no in parts:
                  usedssthits = usedssthits + int(no[1:])

    #--number of events--
    cmd = "cat "+filename+" | grep \"Done with Event\" | wc -l"
    output = commands.getoutput(cmd)
    no_event=int(output)
    row["NoEventDone"]=no_event
    #--memory used --
    cmd = "cat "+filename+" | grep \"EndMaker\" | grep \"total\" | grep \"outputStream\" | awk \'{print $7}\'"
    output = commands.getoutput(cmd)
    parts = output.split("\n")
    if len(parts) > 1:
       first_reading = parts[0]
       length = len(parts)
       last_reading = parts[length-1]
       memFreading= first_reading[1:]
       memLreading = last_reading[1:]
    else: 
       memFreading = "0.00"
       memLreading = "0.00"
    row["memUsageF"]=float(memFreading)
    row["memUsageL"]=float(memLreading)
    #--V0, Xi and Kink vertices--
    v0vertices = 0
    xivertices = 0
    kinkvertices = 0
    if embedflag == 1:
       cmd = "cat "+filename+" | grep \"V0 vertices\" | awk \'{print $7}\'"
       output = commands.getoutput(cmd)
       parts = output.split("\n")
       if len(parts) > 1:
          for no in parts:
              v0vertices = v0vertices + int(no)
       cmd = "cat "+filename+" | grep \"Xi vertices\" | awk \'{print $7}\'"
       output = commands.getoutput(cmd)
       parts = output.split("\n")
       if len(parts) > 1:
          for no in parts:
              xivertices = xivertices + int(no)
       cmd = "cat "+filename+" | grep \"Kink vertices\" | awk \'{print $7}\'"
       output = commands.getoutput(cmd)
       parts = output.split("\n")
       if len(parts) > 1:
          for no in parts:
              kinkvertices = kinkvertices + int(no) 
    #--event reading--
    cmd = "cat "+filename+" | grep \"Reading Event:\" | wc -l"
    output = commands.getoutput(cmd)
    reading_event=int(output)
    #--number of tracks + NFit15--
    cmd = "cat "+filename+" | grep \"track nodes:\" "
    output = commands.getoutput(cmd)
    parts = output.split("\n")
    no_tracks = 0
    no_nfit15 = 0
    no_event_with_track = 0
    if len(parts)>1:
       for no in parts:
        #if no.find("QA")>0:
           newparts = no.split(":")
           ntrack = newparts[2]
           no_tracks = no_tracks + int(ntrack)
           nfit15 = newparts[4]
           no_nfit15 = no_nfit15 + int(nfit15)
           if int(ntrack)>0:
              no_event_with_track = no_event_with_track + 1
    #--number of skipped tracks--
    #cmd = "cat "+filename+" | grep \"INFO  - QAInfo:Run\" | grep \"Total events processed\" | awk \'{print $17}\'"
    cmd = "cat "+filename+" | grep 'StAnalysisMaker::Finish() Processed' | awk '{print $5}'"
    output = commands.getoutput(cmd)
    #print("output:"+output) 
    skip_event=int(output)
    row["NoEventSkip"]=skip_event
    #--Event completed--
    complete_events = no_event - skip_event
    #--calculatiocn of CPU time--
    if embedflag == 0:
       cmd = "cat "+filename+" | grep \"QAInfo:Chain\" | grep \"StBFChain::bfc\""
       output = commands.getoutput(cmd)
       #parts = output.split(" ")
       #total_cpu = float(parts[31][1:])
       #only_cpu = float(parts[40][1:])
    else:    
       cmd = "cat "+filename+" | grep \"QAInfo:Chain\" | grep \"StChain::Embedding\""
       output = commands.getoutput(cmd)
       #parts = output.split(" ")
       #total_cpu = float(parts[31][1:])
       #only_cpu = float(parts[40][1:])
    total_cpu = float(output.split("=")[1].split("Cpu")[0])
    only_cpu = float(output.split("=")[2])
    if complete_events > 0:  
       avg_total_cpu = total_cpu / complete_events
       avg_only_cpu = only_cpu / complete_events
    else:
       avg_total_cpu = 0
       avg_only_cpu = 0
    row["RealTime_per_evt"]=avg_total_cpu
    row["CPU_per_evt_sec"]=avg_only_cpu
    #--other readings--
    if complete_events > 0:
       percent_usable_event = int(round(truediv((no_event_with_track*100),complete_events)))
       avg_tracks = int(round(truediv(no_tracks,complete_events)))
       avg_tracks_nfit15 = int(round(truediv(no_nfit15,complete_events)))
       avg_v0_vertices = int(round(truediv(v0vertices,complete_events)))
       avg_kink_vertices = int(round(truediv(kinkvertices,complete_events)))
       avg_xi_vertices = int(round(truediv(xivertices,complete_events)))
    else:
       percent_usable_event = 0
       avg_tracks = 0
       avg_tracks_nfit15 = 0
       avg_v0_vertices = 0
       avg_kink_vertices = 0
       avg_xi_vertices = 0
    row["percent_of_usable_evt"]=percent_usable_event
    row["avg_no_tracks"]=avg_tracks
    row["avg_no_tracksnfit15"]=avg_tracks_nfit15
    row["avg_no_V0Vrt"]=avg_v0_vertices
    row["avg_no_KinkVrt"]=avg_kink_vertices 
    row["avg_no_XiVrt"]=avg_xi_vertices
    if no_event > 0:
       avg_tracks_per_event = int(round(truediv(no_tracks,no_event_with_track)))
       avg_tracks_nfit15_per_event = int(round(truediv(no_nfit15,no_event_with_track)))
       avg_v0_vertices_per_event = int(round(truediv(v0vertices,no_event_with_track)))
       avg_kink_vertices_per_event = int(round(truediv(kinkvertices,no_event_with_track)))
       avg_xi_vertices_per_event = int(round(truediv(xivertices,no_event_with_track)))
       avg_primary_tracks_1vtx = int(round(truediv(primary_track,no_event_with_track)))
       avg_primary_track_nfit15_1vtx = int(round(truediv(primary_track_nfit15,no_event_with_track)))
       #--QA jobs--
       avg_mtd_hits = int(round(truediv(mtdhits,no_event_with_track)))
       avg_pixel_hits = int(round(truediv(pixelhits,no_event_with_track)))
       avg_ist_hits = int(round(truediv(isthits,no_event_with_track)))
       avg_sst_hits = int(round(truediv(ssthits,no_event_with_track)))
       avg_used_pixel_hits = int(round(truediv(usedpixelhits,no_event_with_track)))
       avg_used_ist_hits = int(round(truediv(usedisthits,no_event_with_track)))
       avg_used_sst_hits = int(round(truediv(usedisthits,no_event_with_track)))
    else:
       avg_tracks_per_event = 0
       avg_tracks_nfit15_per_event = 0
       avg_v0_vertices_per_event = 0
       avg_kink_vertices_per_event = 0
       avg_xi_vertices_per_event = 0
       avg_primary_tracks_1vtx = 0
       avg_primary_track_nfit15_1vtx = 0
       #--QA jobs--
       avg_mtd_hits = 0
       avg_pixel_hits = 0
       avg_ist_hits = 0
       avg_sst_hits = 0
       avg_used_pixel_hits = 0
       avg_used_ist_hits = 0
       avg_used_sst_hits = 0
    row["avgNoTrack_usbevt"]=avg_tracks_per_event
    row["avgNoTrackNfit15_usbevt"]=avg_tracks_nfit15_per_event
    row["avgNoV0_usbevt"]=avg_v0_vertices_per_event
    row["avgNoKink_usbevt"]=avg_kink_vertices_per_event
    row["avgNoXi_usbevt"]=avg_xi_vertices_per_event
    if vrank_no > 0:
       #print(primary_track)
       #print(vrank_no)
       avg_primary_tracks = int(round(truediv(primary_track,vrank_no)))
       avg_primary_track_nfit15 = int(round(truediv(primary_track_nfit15,vrank_no)))
    else:
       avg_primary_tracks = 0
       avg_primary_track_nfit15 = 0
 
    row["avg_no_primaryT"]=avg_primary_tracks
    row["avg_no_primaryT_1vtx"]=avg_primary_tracks
    row["avg_no_primaryTnfit15"]=avg_primary_track_nfit15
    row["avg_no_primaryTnfit15_1vtx"]=avg_primary_track_nfit15
    row["avgNoPrTrack_1vtx_usbevt"]= avg_primary_tracks_1vtx
    row["avgNoPrTrackNfit15_1vtx_usbevt"]= avg_primary_track_nfit15_1vtx
    #--for QA table--
    row["avg_mtd_hits"] = avg_mtd_hits 
    row["avg_pixel_hits"] = avg_pixel_hits 
    row["avg_ist_hits"] = avg_ist_hits 
    row["avg_sst_hits"] = avg_sst_hits
    row["avg_used_pixel_hits"] = avg_used_pixel_hits 
    row["avg_used_ist_hits"] = avg_used_ist_hits 
    row["avg_used_sst_hits"] = avg_used_sst_hits
    return row


def ParseErrorFile(filename):
    #--error messages--
    error_msg = ["bus error", "segmentation violation","Segmentation violation","segmentation fault","Segmentation fault","Stale NFS file handle","Tried to find a host for 500 times, will abort now","Killed","Abort","StFATAL","Catch exception FATAL","floating point exception","Fatal in <operator delete","Fatal in <operator new>","runtime_error","failed"]
    return 0

def CheckDBRecord(parsing_info):
    qry = "select * from JobStatus where "
    qry1 = "LibLevel='"+parsing_info.get("LibLevel")+"' and LibTag='"+parsing_info.get("LibTag")+"' and rootLevel='"+parsing_info.get("rootLevel")+"' and path='"+parsing_info.get("path")+"' and logFile='"+parsing_info.get("logFile")+"' and createTime='"+parsing_info.get("createTime")+"' and chainOpt='"+parsing_info.get("chainOpt")+"'"
    query = qry+qry1
    #print(query)
    result = DBFun.execute("LibraryJobs",query)
    count = 0
    for row in result:
        count = count + 1
    return count

def InsertDBRecords(parsing_info):
    valuelist = "("
    qry1 = "insert into JobStatus (jobID,LibLevel,LibTag,rootLevel,path,prodyear,logFile,createTime,chainOpt,jobStatus,crashedCode,errMessage,NoEventDone,memUsageF,memUsageL,CPU_per_evt_sec,RealTime_per_evt,percent_of_usable_evt,avg_no_tracks,avg_no_tracksnfit15,NoEventVtx,avgNoVtx_evt,avg_no_primaryT,avg_no_primaryT_1vtx,avg_no_primaryTnfit15,avg_no_primaryTnfit15_1vtx,avg_no_V0Vrt,avg_no_XiVrt,avg_no_KinkVrt,avgNoTrack_usbevt,avgNoTrackNfit15_usbevt,avgNoPrTrack_1vtx_usbevt,avgNoPrTrackNfit15_1vtx_usbevt,avgNoV0_usbevt,avgNoXi_usbevt,avgNoKink_usbevt,nodeID,NoEventSkip,avg_no_btof) values "
    keys = qry1.split("(")[1].split(")")[0].split(",")
    for key in keys:
        if type(parsing_info.get(key))==str:
           valuelist = valuelist +"'"+str(parsing_info.get(key))+"',"
        else:
           valuelist = valuelist+str(parsing_info.get(key))+","
    valuelist = valuelist[:-1]
    valuelist = valuelist + ")"
    #print(qry1+valuelist) 
    query = qry1+valuelist
    result = DBFun.execute("LibraryJobs",query)
    print("Added the row...")
    return 0

def ForEachFile(filename):
      cmd = "cat "+filename+" | egrep '(Run completed|Terminating)'"
      output = commands.getoutput(cmd)
      if output.find("Run completed")<0 and output.find("Terminating")<0 :
         cmd = "cat "+filename+" | grep \"error\""
         output = commands.getoutput(cmd)
         print("-------------------------------------------------------------------------------------------------")
         print("Error:"+output)
         print("-------------------------------------------------------------------------------------------------")
      else:
         print("Successfully completed...")
         parsing_info = Parse(filename)
         print("-------------------------------------------------------------------------------------------------")
         print(parsing_info)
         print("-------------------------------------------------------------------------------------------------")
         if CheckDBRecord(parsing_info)==0:
            InsertDBRecords(parsing_info)
         else:
            print("Row is already present in DB...")
         print("-------------------------------------------------------------------------------------------------") 
         #sort_keys = parsing_info.keys()
         #sort_keys.sort()
         #for parameter in sort_keys:
         #    print('|%32s:%s'%(str(parameter),str(parsing_info.get(parameter))))
         #    print("-------------------------------------------------------------------------------------------------") 
         #exit()

if __name__ == "__main__":
   if len(sys.argv)!=2:
      print("------------------------------------------------")
      print("....Please use like this........................")
      print("    python ParseLogFile.py /abc/pqr/filelist.txt")
      print("------------------------------------------------")
   else:
      filelist = open(sys.argv[1],"rb+")
      for fl in filelist:
          print("File:"+fl[:-1])
          ForEachFile(fl[:-1])





