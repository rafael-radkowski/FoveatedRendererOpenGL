import os
import time
import sys

width = 1024
height = 1024

window = [1024, 2048]
fbo_resolution = [32, 64, 128, 256, 512, 1024, 2048, 4096]
eye_radius = [0.1, 0.2, 0.4]
num_objects = [1, 2, 3]



def getValues():

	path = 'result'

	with open(path) as fp:
		line = fp.readline()
		mean = line.strip()
		print("Average all: {}".format( mean))
		line = fp.readline()
		swap = line.strip()
		print("Average swap: {}".format( swap))
		
	#os.remove(path)
		
	return float(mean), float(swap)
		
		#cnt = 1
		#while line:
	#		print("Line {}: {}".format(cnt, line.strip()))
	#		line = fp.readline()
	#		cnt += 1


def run_test(output_folder):
	"""
	Run a set of tests to get the performance of the descriptors
	model_path - absolute or relative model path
	output_folder - folder to write the results into
	num_tests - nuber of tests to run. 
	"""
	log_path = '../bin/'
	log_path = log_path + output_folder + '/'
	log_file = 'foveated_rendering_log_' 
	log_type = '.csv'
	
	print("log_path  %s " % log_path)

	try:
		os.mkdir(log_path)
	except OSError:
		print("Creation of the directory %s failed" % log_path)
	else:
		print("Successfully created the directory %s " % log_path)

	header = 'idx,fr,window,tex,radius,num,duration,all_mean,swap_mean'
	f = open(str(log_path + "summary_log.csv"), "a+")
	f.write(header + "\n")
	f.close()

	#-w 1024 -h 1024 -tex 32 -eye 0.1 -o ./data/test3.csv -end 5000 -fr

	count = 0
	for w in window:
	
		for num_obj in num_objects:
	
			# ----------------------------
			# start with full resolution renderer 
			log = output_folder + '/'+ log_file + str(count) + log_type
			
			start_time = time.time()		
			os.system('..\\bin\\FoveatedRendererOpenGL.exe -w ' + str(w) + ' -h ' + str(w) +  str(log)+ ' -end 5000  -num ' + str(num_obj) )
			duration = time.time() - start_time
			print(duration)

			avg, swp = getValues()

			f = open(str(log_path + "summary_log.csv"), "a+")
			f.write(str(count) + "," + str(0) + "," + str(w) + "," + str(0) + "," + str(0.1) + "," + str(num_obj) + "," + str(duration)+ "," + str(avg)+ "," + str(swp)+ "\n")
			f.close()
			count = count + 1
		
	
			for res in fbo_resolution:

				
				# ----------------------------
				# start with FR 
				log = log_path + log_file + str(count) + log_type
				
				start_time = time.time()		
				os.system('..\\bin\\FoveatedRendererOpenGL.exe -w ' + str(w) + ' -h ' + str(w) +  ' -tex ' + str(res) + ' -eye 0.1 -o ' +  str(log)+ ' -end 5000  -num ' + str(num_obj) + ' -fr')
				duration = time.time() - start_time
				print(duration)
				
				avg, swp = getValues()

				f = open(str(log_path + "summary_log.csv"), "a+")
				f.write(str(count) + "," + str(1) + "," + str(w) + "," + str(res) + "," + str(0.1) + "," + str(num_obj) + "," + str(duration)+ "," + str(avg)+ "," + str(swp)+ "\n")
				f.close()
				count = count + 1
			
			
			
			
			
				
		



def main():

	#print("ref %s " % sys.argv[1])
	#print("test %s " % sys.argv[2])
	#print("output %s " % sys.argv[3])
	#print("num test %s " % sys.argv[4])
	
	#getValues()
	run_test("data")




if __name__ == "__main__":
	main()

