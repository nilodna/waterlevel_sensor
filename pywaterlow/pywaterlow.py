import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import datetime
import os

plt.close('all')

#
# class WaterLow(object):
#     """docstring for ."""
#     def __init__(self, base, fname):
#         self.base_dir   = base
#         self.base_fname = fname
#         self.fullpath   = os.path.join(self.base_dir,self.base_fname)
#
#     def split_datalog(self,header,csv=False):
#         f = open(self.fullpath)
#         count = 0
#
#         for line in f.readlines():
#             if line.replace('\n','') == header:
#                 print('Novo arquivo gerado!')
#                 # fecha, caso tenha aberto, um arquivo
#                 # o.close()
#                 # abre novo arquivo
#                 newfilename = 'raw_datalog_' + str(count) + '.txt'
#                 newfile = os.path.join(BASE_DIR,'data',newfilename)
#                 o = open(newfile,'w')
#                 # salva a primeira linha em header
#                 if csv:
#                     o.write(line.replace(' ',';'))
#                 else:
#                     o.write(line)
#                 # counting for a new file
#                 count += 1
#             else:
#                 # salva as novas linhas no arquivo aberto
#                 if csv:
#                     o.write(line.replace(' ',';'))
#                 else:
#                     o.write(line)
#
#         f.close()
#
#
# header = 'Ano Mes Dia H M S Dist(cm) Dist2(cm) T(°C) P(Pa)'
# base = os.getcwd().replace('pywaterlow','arraial')
# fname= 'data/raw_datalog.txt'
#
# data1 = WaterLow(base,fname)
# data1.split_datalog(header,csv=True)

# funcao para separar os dados por header
def split_datalog(fname,header,csv=False):
    print(header)

    f = open(fname)
    count = 0

    for line in f.readlines():
        if line.replace('\n','') == header:
            print('Novo arquivo gerado!')
            # fecha, caso tenha aberto, um arquivo
            # o.close()
            # abre novo arquivo
            newfilename = 'raw_datalog_' + str(count) + '.txt'
            newfile = os.path.join(BASE_DIR,'data',newfilename)
            o = open(newfile,'w')
            # salva a primeira linha em header
            if csv:
                o.write(line.replace(' ',';'))
            else:
                o.write(line)
            # counting for a new file
            count += 1
        else:
            # salva as novas linhas no arquivo aberto
            if csv:
                o.write(line.replace(' ',';'))
            else:
                o.write(line)

    f.close()

def create_dates(fname):

    ncin = np.loadtxt(fname,delimiter=';',usecols=[0,1,2,3,4,5],skiprows=1)

    dates = []
    years = ncin[:,0]
    month = ncin[:,1]
    days  = ncin[:,2]
    hours = ncin[:,3]
    minut = ncin[:,4]
    secos = ncin[:,5]

    for y,m,d,h,mi,secs in zip(years,month,days,hours,minut,secos):
        dates.append(datetime.datetime(int(y),int(m),int(d),int(h),int(mi),int(secs)))

    return np.asarray(dates)

def read_files(fname,count):
    # count é o numero do arquivo que deve ser lido
    newfile = '_' + str(count) + '.txt'

    date = pd.DatetimeIndex(create_dates(fname.replace('.txt',newfile)))

    data = pd.read_csv(fname.replace('.txt',newfile),header=0,sep=';',usecols=[6,7])
    data.index= date

    return data

def quality_control(df,key):

    # df[key] = df[key].mask(df[key] > 3*df[key].std())
    # df[key] = df[key].mask(df[key] < -3*df[key].std())
    df[key] = df[key].mask(df[key] < 20)
    df[key] = df[key].mask(df[key] > 150)

    return df

def view(df1,df2):

    fig,axes = plt.subplots(nrows=2,figsize=(25/2.54,10/2.54))

    df1.plot(ax=axes[0])
    df2.plot(ax=axes[1])

    axes[0].grid('--',alpha=.3)
    axes[1].grid('--',alpha=.3)

    axes[0].set_title('Ponto - Esgoto',fontsize=10)
    axes[1].set_title('Ponto - Praia',fontsize=10)

    plt.tight_layout()

    outDir = os.getcwd().replace('pywaterlow','arraial')
    plt.savefig(os.path.join(outDir,'outputs','arraial01.png'),dpi=150)

## MAIN
BASE_DIR = os.getcwd().replace('pywaterlow','arraial')
fname = os.path.join(BASE_DIR,'data','raw_datalog.txt')
header = 'Ano Mes Dia H M S Dist(cm) Dist2(cm) T(°C) P(Pa)'

# descomentar se quiser gerar um novo split do raw_datalog.txt
# split_datalog(fname,header,csv=True)

# 3 e 4 pq eu sei que foi o terceiro bloco de dados válidos

data_esgoto = read_files(fname,3)
data_praia  = read_files(fname,4)

# removendo outliers
data_esgoto = quality_control(data_esgoto,'Dist2(cm)')
data_praia  = quality_control(data_praia,'Dist2(cm)')

# renaming columns
data_esgoto.columns = ['HC-SR04','Sharp']
data_praia.columns = ['HC-SR04','Sharp']

# alisando
data_esgoto = data_esgoto.resample('1S').mean()
data_praia = data_praia.resample('1S').mean()

# visualization
view(data_esgoto,data_praia)
