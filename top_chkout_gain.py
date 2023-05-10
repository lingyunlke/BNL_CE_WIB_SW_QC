from wib_cfgs import WIB_CFGS
import time
import sys
import numpy as np
import pickle
import copy
import time, datetime, random, statistics

if len(sys.argv) < 2:
    print('Please specify at least one FEMB # to test')
    print('Usage: python wib.py 0')
    exit()    

if 'save' in sys.argv:
    save = True
    for i in range(len(sys.argv)):
        if sys.argv[i] == 'save':
            pos = i
            break
    sample_N = int(sys.argv[pos+1] )
    if sample_N < 10:
        sample_N = 10
    sys.argv.remove('save')
else:
    save = False
    sample_N = 10

fembs = [int(a) for a in sys.argv[1:pos]] 
print (fembs)

chk = WIB_CFGS()

for snc in [1,0]:
    for dac in range(64):
    #for st0 in [0,1]:
    #    for st1 in [0,1]:
        for sts in [1]:
            ####################WIB init################################
            #check if WIB is in position
            chk.wib_fw()
            ####################FEMBs Configuration################################
            #step 1
            #reset all FEMBs on WIB
            chk.femb_cd_rst()
            
            cfg_paras_rec = []
            for femb_id in fembs:
            #step 2
            #Configur Coldata, ColdADC, and LArASIC parameters. 
            #Here Coldata uses default setting in the script (not the ASIC default register values)
            #ColdADC configuraiton
                chk.adcs_paras = [ # c_id, data_fmt(0x89), diff_en(0x84), sdc_en(0x80), vrefp, vrefn, vcmo, vcmi, autocali
                                    [0x4, 0x08, 0, 0, 0xDF, 0x33, 0x89, 0x67, 1],
                                    [0x5, 0x08, 0, 0, 0xDF, 0x33, 0x89, 0x67, 1],
                                    [0x6, 0x08, 0, 0, 0xDF, 0x33, 0x89, 0x67, 1],
                                    [0x7, 0x08, 0, 0, 0xDF, 0x33, 0x89, 0x67, 1],
                                    [0x8, 0x08, 0, 0, 0xDF, 0x33, 0x89, 0x67, 1],
                                    [0x9, 0x08, 0, 0, 0xDF, 0x33, 0x89, 0x67, 1],
                                    [0xA, 0x08, 0, 0, 0xDF, 0x33, 0x89, 0x67, 1],
                                    [0xB, 0x08, 0, 0, 0xDF, 0x33, 0x89, 0x67, 1],
                                  ]
            
            #LArASIC register configuration
                chk.set_fe_board(sts=1, snc=snc,sg0=0, sg1=0, st0=1, st1=1, swdac=1, sdd=0,dac=dac )
                adac_pls_en = 1 #enable LArASIC interal calibraiton pulser
                cfg_paras_rec.append( (femb_id, copy.deepcopy(chk.adcs_paras), copy.deepcopy(chk.regs_int8), adac_pls_en) )
            #step 3
                chk.femb_cfg(femb_id, adac_pls_en )
            
            chk.data_align(fembs)
            
            time.sleep(0.5)
            
            ####################FEMBs Data taking################################
            rawdata = chk.spybuf_trig(fembs=fembs, num_samples=sample_N, trig_cmd=0x00) #returns list of size 1
            
            pwr_meas = chk.get_sensors()
            
            if save:
                fdir = "./tmp_data/"
                ts = datetime.datetime.now().strftime("%d_%m_%Y_%H_%M_%S")
                fp = fdir + "Raw_gain_snc%d_dac0%d_"%(snc,dac) + ts  + ".bin"
                with open(fp, 'wb') as fn:
                    pickle.dump( [rawdata, pwr_meas, cfg_paras_rec], fn)

