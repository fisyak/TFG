TDataSet *CreateTable() { 
// -----------------------------------------------------------------
// bfc/.make/db/.const/StarDb/Geometry/tpc/.TpcSuperSectorPositionB/TpcSuperSectorPositionB Allocated rows: 24  Used rows: 24  Row size: 184 bytes
//  Table: Survey_st[0]--> Survey_st[23]
// ====================================================================
// ------  Test whether this table share library was loaded ------
  if (!TClass::GetClass("St_Survey")) return 0;
Survey_st row;
St_Survey *tableSet = new St_Survey("TpcSuperSectorPositionB",24);
//
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =          1; // ;
    row.r00	 =          1; // ;
    row.r01	 =    0.00037; // -gamma ;
    row.r02	 =    -0.0004; // beta  ;
    row.r10	 =   -0.00037; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =     -8e-05; // -alpha ;
    row.r20	 =     0.0004; // -beta  ;
    row.r21	 =      6e-05; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =    -0.0151; // ;
    row.t1	 =     0.1454; // ;
    row.t2	 =    -0.0358; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00013; // ;
    row.sigmaTrX	 =      9e-05; // ;
    row.sigmaTrY	 =    0.00021; // ;
    row.sigmaTrZ	 =    0.00011; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =          2; // ;
    row.r00	 =          1; // ;
    row.r01	 =    0.00191; // -gamma ;
    row.r02	 =   -0.00035; // beta  ;
    row.r10	 =   -0.00191; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =      6e-05; // -alpha ;
    row.r20	 =    0.00035; // -beta  ;
    row.r21	 =     -6e-05; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =     0.0334; // ;
    row.t1	 =     0.1879; // ;
    row.t2	 =    -0.0558; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00013; // ;
    row.sigmaTrX	 =      9e-05; // ;
    row.sigmaTrY	 =    0.00021; // ;
    row.sigmaTrZ	 =     0.0001; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =          3; // ;
    row.r00	 =          1; // ;
    row.r01	 =    0.00237; // -gamma ;
    row.r02	 =    -0.0001; // beta  ;
    row.r10	 =   -0.00237; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =     -9e-05; // -alpha ;
    row.r20	 =     0.0001; // -beta  ;
    row.r21	 =      9e-05; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =     0.0887; // ;
    row.t1	 =     0.1416; // ;
    row.t2	 =    -0.0501; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00013; // ;
    row.sigmaTrX	 =      9e-05; // ;
    row.sigmaTrY	 =    0.00021; // ;
    row.sigmaTrZ	 =    0.00011; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =          4; // ;
    row.r00	 =          1; // ;
    row.r01	 =    0.00406; // -gamma ;
    row.r02	 =      3e-05; // beta  ;
    row.r10	 =   -0.00406; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =    0.00013; // -alpha ;
    row.r20	 =     -3e-05; // -beta  ;
    row.r21	 =   -0.00013; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =     0.0832; // ;
    row.t1	 =     0.1746; // ;
    row.t2	 =    -0.0368; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00014; // ;
    row.sigmaTrX	 =      9e-05; // ;
    row.sigmaTrY	 =    0.00021; // ;
    row.sigmaTrZ	 =    0.00011; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =          5; // ;
    row.r00	 =          1; // ;
    row.r01	 =    0.00195; // -gamma ;
    row.r02	 =    0.00014; // beta  ;
    row.r10	 =   -0.00195; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =     -7e-05; // -alpha ;
    row.r20	 =   -0.00014; // -beta  ;
    row.r21	 =      7e-05; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =     0.0451; // ;
    row.t1	 =     0.0934; // ;
    row.t2	 =    -0.0489; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00013; // ;
    row.sigmaTrX	 =     0.0001; // ;
    row.sigmaTrY	 =    0.00023; // ;
    row.sigmaTrZ	 =    0.00011; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =          6; // ;
    row.r00	 =          1; // ;
    row.r01	 =    0.00085; // -gamma ;
    row.r02	 =    0.00016; // beta  ;
    row.r10	 =   -0.00085; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =     -9e-05; // -alpha ;
    row.r20	 =   -0.00016; // -beta  ;
    row.r21	 =      9e-05; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =     0.0688; // ;
    row.t1	 =     0.0336; // ;
    row.t2	 =    -0.0913; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00013; // ;
    row.sigmaTrX	 =     0.0001; // ;
    row.sigmaTrY	 =    0.00022; // ;
    row.sigmaTrZ	 =    0.00011; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =          7; // ;
    row.r00	 =          1; // ;
    row.r01	 =    0.00212; // -gamma ;
    row.r02	 =    0.00017; // beta  ;
    row.r10	 =   -0.00212; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =    0.00012; // -alpha ;
    row.r20	 =   -0.00017; // -beta  ;
    row.r21	 =   -0.00012; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =     0.0616; // ;
    row.t1	 =     0.0517; // ;
    row.t2	 =    -0.0889; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00013; // ;
    row.sigmaTrX	 =     0.0001; // ;
    row.sigmaTrY	 =    0.00022; // ;
    row.sigmaTrZ	 =    0.00011; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =          8; // ;
    row.r00	 =          1; // ;
    row.r01	 =    0.00251; // -gamma ;
    row.r02	 =   -0.00019; // beta  ;
    row.r10	 =   -0.00251; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =      9e-05; // -alpha ;
    row.r20	 =    0.00019; // -beta  ;
    row.r21	 =     -9e-05; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =     0.0326; // ;
    row.t1	 =     0.0444; // ;
    row.t2	 =    -0.1088; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00013; // ;
    row.sigmaTrX	 =      9e-05; // ;
    row.sigmaTrY	 =    0.00022; // ;
    row.sigmaTrZ	 =     0.0001; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =          9; // ;
    row.r00	 =          1; // ;
    row.r01	 =   -0.00068; // -gamma ;
    row.r02	 =   -0.00021; // beta  ;
    row.r10	 =    0.00068; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =   -0.00037; // -alpha ;
    row.r20	 =    0.00021; // -beta  ;
    row.r21	 =    0.00035; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =     0.0382; // ;
    row.t1	 =    -0.0675; // ;
    row.t2	 =    -0.0566; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00013; // ;
    row.sigmaTrX	 =      9e-05; // ;
    row.sigmaTrY	 =    0.00021; // ;
    row.sigmaTrZ	 =    0.00011; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =         10; // ;
    row.r00	 =          1; // ;
    row.r01	 =   -0.00049; // -gamma ;
    row.r02	 =   -0.00041; // beta  ;
    row.r10	 =    0.00049; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =   -0.00031; // -alpha ;
    row.r20	 =    0.00041; // -beta  ;
    row.r21	 =    0.00031; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =     0.0084; // ;
    row.t1	 =     0.0244; // ;
    row.t2	 =     0.0053; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00013; // ;
    row.sigmaTrX	 =      9e-05; // ;
    row.sigmaTrY	 =    0.00021; // ;
    row.sigmaTrZ	 =    0.00011; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =         11; // ;
    row.r00	 =          1; // ;
    row.r01	 =    0.00089; // -gamma ;
    row.r02	 =   -0.00033; // beta  ;
    row.r10	 =   -0.00089; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =   -0.00026; // -alpha ;
    row.r20	 =    0.00033; // -beta  ;
    row.r21	 =    0.00026; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =     0.0018; // ;
    row.t1	 =     0.0637; // ;
    row.t2	 =    -0.0162; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00013; // ;
    row.sigmaTrX	 =     0.0001; // ;
    row.sigmaTrY	 =    0.00022; // ;
    row.sigmaTrZ	 =    0.00011; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =         12; // ;
    row.r00	 =          1; // ;
    row.r01	 =    0.00428; // -gamma ;
    row.r02	 =    -0.0003; // beta  ;
    row.r10	 =   -0.00428; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =    0.00023; // -alpha ;
    row.r20	 =     0.0003; // -beta  ;
    row.r21	 =   -0.00023; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =     0.0232; // ;
    row.t1	 =     0.1753; // ;
    row.t2	 =    -0.0072; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00012; // ;
    row.sigmaTrX	 =     0.0001; // ;
    row.sigmaTrY	 =    0.00022; // ;
    row.sigmaTrZ	 =    0.00011; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =         13; // ;
    row.r00	 =          1; // ;
    row.r01	 =   -0.00196; // -gamma ;
    row.r02	 =   -0.00015; // beta  ;
    row.r10	 =    0.00196; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =      2e-05; // -alpha ;
    row.r20	 =    0.00015; // -beta  ;
    row.r21	 =     -2e-05; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =    -0.0053; // ;
    row.t1	 =    -0.0833; // ;
    row.t2	 =    -0.0575; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00014; // ;
    row.sigmaTrX	 =      9e-05; // ;
    row.sigmaTrY	 =    0.00022; // ;
    row.sigmaTrZ	 =    0.00011; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =         14; // ;
    row.r00	 =          1; // ;
    row.r01	 =   -0.00345; // -gamma ;
    row.r02	 =     -3e-05; // beta  ;
    row.r10	 =    0.00345; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =     -5e-05; // -alpha ;
    row.r20	 =      3e-05; // -beta  ;
    row.r21	 =      5e-05; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =    -0.0059; // ;
    row.t1	 =    -0.2073; // ;
    row.t2	 =    -0.0727; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00014; // ;
    row.sigmaTrX	 =      9e-05; // ;
    row.sigmaTrY	 =    0.00021; // ;
    row.sigmaTrZ	 =    0.00011; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =         15; // ;
    row.r00	 =          1; // ;
    row.r01	 =   -0.00395; // -gamma ;
    row.r02	 =      7e-05; // beta  ;
    row.r10	 =    0.00395; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =     -8e-05; // -alpha ;
    row.r20	 =     -7e-05; // -beta  ;
    row.r21	 =      8e-05; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =     0.0436; // ;
    row.t1	 =    -0.2637; // ;
    row.t2	 =    -0.0534; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00014; // ;
    row.sigmaTrX	 =      9e-05; // ;
    row.sigmaTrY	 =     0.0002; // ;
    row.sigmaTrZ	 =     0.0001; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =         16; // ;
    row.r00	 =          1; // ;
    row.r01	 =   -0.00131; // -gamma ;
    row.r02	 =    0.00016; // beta  ;
    row.r10	 =    0.00131; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =      5e-05; // -alpha ;
    row.r20	 =   -0.00016; // -beta  ;
    row.r21	 =     -7e-05; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =     0.0672; // ;
    row.t1	 =    -0.2084; // ;
    row.t2	 =    -0.0348; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00014; // ;
    row.sigmaTrX	 =      8e-05; // ;
    row.sigmaTrY	 =     0.0002; // ;
    row.sigmaTrZ	 =     0.0001; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =         17; // ;
    row.r00	 =          1; // ;
    row.r01	 =    -0.0022; // -gamma ;
    row.r02	 =    0.00015; // beta  ;
    row.r10	 =     0.0022; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =    -0.0002; // -alpha ;
    row.r20	 =   -0.00014; // -beta  ;
    row.r21	 =    0.00019; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =     0.0962; // ;
    row.t1	 =    -0.1672; // ;
    row.t2	 =    -0.0444; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00013; // ;
    row.sigmaTrX	 =      9e-05; // ;
    row.sigmaTrY	 =     0.0002; // ;
    row.sigmaTrZ	 =    0.00011; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =         18; // ;
    row.r00	 =          1; // ;
    row.r01	 =   -0.00314; // -gamma ;
    row.r02	 =     -7e-05; // beta  ;
    row.r10	 =    0.00314; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =   -0.00021; // -alpha ;
    row.r20	 =      7e-05; // -beta  ;
    row.r21	 =    0.00021; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =     0.1002; // ;
    row.t1	 =     -0.231; // ;
    row.t2	 =    -0.0641; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00014; // ;
    row.sigmaTrX	 =      9e-05; // ;
    row.sigmaTrY	 =    0.00021; // ;
    row.sigmaTrZ	 =    0.00011; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =         19; // ;
    row.r00	 =          1; // ;
    row.r01	 =   -0.00192; // -gamma ;
    row.r02	 =   -0.00022; // beta  ;
    row.r10	 =    0.00192; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =      2e-05; // -alpha ;
    row.r20	 =    0.00022; // -beta  ;
    row.r21	 =          0; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =     0.0787; // ;
    row.t1	 =    -0.1575; // ;
    row.t2	 =    -0.0347; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00013; // ;
    row.sigmaTrX	 =      9e-05; // ;
    row.sigmaTrY	 =     0.0002; // ;
    row.sigmaTrZ	 =    0.00011; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =         20; // ;
    row.r00	 =          1; // ;
    row.r01	 =   -0.00155; // -gamma ;
    row.r02	 =   -0.00041; // beta  ;
    row.r10	 =    0.00155; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =     -3e-05; // -alpha ;
    row.r20	 =    0.00041; // -beta  ;
    row.r21	 =      3e-05; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =     0.0791; // ;
    row.t1	 =    -0.0858; // ;
    row.t2	 =    -0.0481; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00013; // ;
    row.sigmaTrX	 =      9e-05; // ;
    row.sigmaTrY	 =     0.0002; // ;
    row.sigmaTrZ	 =     0.0001; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =         21; // ;
    row.r00	 =          1; // ;
    row.r01	 =    0.00039; // -gamma ;
    row.r02	 =   -0.00034; // beta  ;
    row.r10	 =   -0.00039; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =     -7e-05; // -alpha ;
    row.r20	 =    0.00034; // -beta  ;
    row.r21	 =      7e-05; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =     0.0636; // ;
    row.t1	 =    -0.0602; // ;
    row.t2	 =    -0.0474; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00013; // ;
    row.sigmaTrX	 =      9e-05; // ;
    row.sigmaTrY	 =     0.0002; // ;
    row.sigmaTrZ	 =     0.0001; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =         22; // ;
    row.r00	 =          1; // ;
    row.r01	 =   -0.00127; // -gamma ;
    row.r02	 =   -0.00034; // beta  ;
    row.r10	 =    0.00127; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =     -1e-05; // -alpha ;
    row.r20	 =    0.00034; // -beta  ;
    row.r21	 =      1e-05; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =     0.0295; // ;
    row.t1	 =    -0.0428; // ;
    row.t2	 =    -0.0258; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00013; // ;
    row.sigmaTrX	 =      9e-05; // ;
    row.sigmaTrY	 =     0.0002; // ;
    row.sigmaTrZ	 =     0.0001; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =         23; // ;
    row.r00	 =          1; // ;
    row.r01	 =   -0.00073; // -gamma ;
    row.r02	 =   -0.00033; // beta  ;
    row.r10	 =    0.00073; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =      1e-05; // -alpha ;
    row.r20	 =    0.00033; // -beta  ;
    row.r21	 =     -1e-05; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =     0.0161; // ;
    row.t1	 =    -0.0064; // ;
    row.t2	 =    -0.0555; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00014; // ;
    row.sigmaTrX	 =      9e-05; // ;
    row.sigmaTrY	 =     0.0002; // ;
    row.sigmaTrZ	 =    0.00011; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
memset(&row,0,tableSet->GetRowSize());
    row.Id	 =         24; // ;
    row.r00	 =          1; // ;
    row.r01	 =   -0.00296; // -gamma ;
    row.r02	 =   -0.00022; // beta  ;
    row.r10	 =    0.00296; // gamma ;
    row.r11	 =          1; // ;
    row.r12	 =   -0.00011; // -alpha ;
    row.r20	 =    0.00022; // -beta  ;
    row.r21	 =    0.00011; // alpha ;
    row.r22	 =          1; // ;
    row.t0	 =    -0.0214; // ;
    row.t1	 =    -0.0751; // ;
    row.t2	 =    -0.0246; // ;
    row.sigmaRotX	 =          0; // ;
    row.sigmaRotY	 =          0; // ;
    row.sigmaRotZ	 =    0.00013; // ;
    row.sigmaTrX	 =     0.0001; // ;
    row.sigmaTrY	 =    0.00022; // ;
    row.sigmaTrZ	 =    0.00012; // ;
 memcpy(&row.comment,"Pass510AVGF\x20StiCA.Minuit",24);// 
tableSet->AddAt(&row);
// ----------------- end of code ---------------
 return (TDataSet *)tableSet;
}
