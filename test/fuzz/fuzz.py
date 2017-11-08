# Avery simple fuzzer using Radamsa
#
# Usage:
#  1. compile radamsa and point to it below
#  2. set number of rounds
#  3. (optional) set block template below
#  4. profit

import os
import subprocess

# You have to get (compile) your own copy of radamsa and point to it here
RADAMSA = "./radamsa"

TMPLT_BLOCK 	= "./template_block.bin"			# Template block for radamsa
TEST_BLOCK 		= "./test_block.bin"				# Test block for the parser (will be created by radamsa)
CRASH_BLOCK 	= "./crashed_blocks/crash_block_"	# To store test blocks when the parser crashes
PARSER 			= "./../../bin/parser"				# Where parser binary is located

ROUNDS = 10000

crash_counter = 0

for i in range(0, ROUNDS):
	# print stats from time to time
	if i % 1000 == 0:
		print("\n%i crashes reported so far.\n")

	print("\rRound %i..." % (i + 1), end="")

	# get fresh random block through radamsa
	os.system("cat %s | %s > %s" % (TMPLT_BLOCK, RADAMSA, TEST_BLOCK))

	# launch the vulnerable process, monitor it for crashes
	parser = [PARSER, TEST_BLOCK, TEST_BLOCK]
	try:
		subprocess.check_call(parser, stdout=open("/dev/null", "wb"), stderr=open("/dev/null", "wb"))
	except subprocess.CalledProcessError as e:
		# parser crashed, yay!
		# count the crash
		crash_counter += 1

		# copy the block the program crashed on for future reference
		crash_block_copy_name = CRASH_BLOCK + str(crash_counter)
		os.system("cp %s %s" % (TEST_BLOCK, crash_block_copy_name))

print("\nAll done, crashes reported: %i" % crash_counter)