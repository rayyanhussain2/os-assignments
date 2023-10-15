import matplotlib.pyplot as plt
process_names = ["SCHED FIFO","SCHED RR","SCHED OTHERS",]
fp = open("output.txt","r")
line = fp.readline()
main_list =[]
while line:
    line = line.rstrip()
    main_list.append(float(line))
    line = fp.readline()
plt.bar(process_names,main_list,color=["blue","green","red"])
plt.xlabel("Processes")
plt.ylabel("Execution time (in Seconds)")
plt.ylim(0, 40)
plt.title('Execution time for different scheduling Policies')
plt.show()
