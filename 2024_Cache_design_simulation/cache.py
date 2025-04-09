# -*- coding: utf-8 -*-
"""
Created on Wed Mar  6 19:53:11 2024

@author: 
    Dmitrii Fotin       <dfotin@pdx.edu>
    John Michael Mertz  <jmertz@pdx.edu>  
    Bulou Tian          <bulou@pdx.edu>
    Josh Varughese       <jv23@pdx.edu>
"""

# function to convert decimal MESI values to letter values to print out
def convert_mesi(value):
    """
    Converts Mesi input as Mesi Characters.
    Input integer.
    """
    if value == 0:
        return "I"
    elif value == 1:
        return "S"
    elif value == 2:
        return "E"
    elif value == 3:
        return "M"
    else:
        return "Invalid Mesi"
#general variables that defina cache size
data_ways = 8
inst_ways = 4
capacity = 16384

while True:
    #cache stats to output
    data_num_cache_read = 0
    data_num_cache_write = 0
    data_num_cache_hit = 0
    data_num_cache_miss = 0
    data_cache_hit_ratio = 0.0

    inst_num_cache_read = 0
    inst_num_cache_hit = 0
    inst_num_cache_miss = 0
    inst_cache_hit_ratio = 0.0
    
    #file input
    file_input = input("Enter file name: ")
    #ask if L2 comms should be printed, error check if invalid input
    print_message = input("Would you like to print Communications with l2 Cache? y/n\n")
    while print_message != 'y' and print_message != 'n':
        print_message = input("Invalid Input! Would you like to print Communications with l2 Cache? y/n\n") 
    
    #create the 3D arrays for caches to store tag, LRU, MESI and hex address info
    data_cache = [[['empty' for _ in range(4)] for _ in range(data_ways)] for _ in range(capacity)] #3D array: 16k indices x 8 ways x three values (tag, LRU, MESI)
    instruction_cache = [[['empty' for _ in range(4)] for _ in range(inst_ways)] for _ in range(capacity)] #3D array: 16k indices x 8 ways x three values (tag, LRU, MESI)
    #open the specified file
    with open(file_input, 'r') as file:
        while True:
            #read file line by line
            data = file.readline()
            #if file is empty, exit and ask for next file input
            if not data:
                break
            #if last line in file reached, ask whether to exit the program
            #or enter next file name
            if data == "\n":
                print("Empty Line reached.")
                print("Continue? y/n")
                if input() == 'n':
                    break
                mode = None
            #for commands that are not 8 or 9, unpack line into command and hex address
            elif data != '9\n' and data != '9' and data != '8\n' and data != '8':
                data_unpack = data.split()
                #store command for future branching (0,1,2,3,4,8,9)
                mode = int(data_unpack[0])
                #save hex address, fill with 0s until it's 8 chars long
                address = data_unpack[1].zfill(8)
                #convert address to binary
                binary = ''.join(bin(int(i,16))[2:].zfill(4) for i in address)
                #parse out the tag and set index
                tag = binary[0:12]
                index = binary[12:26].zfill(16)                                                 # Adds leading 0.
                #convert set index to decimal to addres 2^14 array rows
                index_decimal = int(index,2)
            #for commands 8 and 9, there is no tag, set or byte offset
            #specify respective variables as empty
            elif data == '9\n' or data == '9':
                mode = 9
                tag = 'empty'
                index_decimal = 'empty'
            else:
                mode = 8
                tag = 'empty'
                index_decimal = 'empty'
            
            #command 0 - read to data cache
            if mode == 0:
                #increment respective stats
                data_num_cache_read += 1
                tag_exists = False
                # if cache hit, update LRU (for all ways) and MESI (for specified way only)
                for i in range(len(data_cache[index_decimal])):
                    if data_cache[index_decimal][i][0] == tag and data_cache[index_decimal][i][2] != 0:
                        tag_exists = True
                        LRU = data_cache[index_decimal][i][1]
                        for ii in range(data_ways):
                            if data_cache[index_decimal][ii][1] != "empty":                                                                                 # Added: Compares the empty string first before comparing LRUs
                                if data_cache[index_decimal][ii][1] > LRU:
                                    data_cache[index_decimal][ii][1] -= 1
                        data_cache[index_decimal][i][1] = 7
                        if data_cache[index_decimal][i][2] != 3:
                            data_cache[index_decimal][i][2] = 1     # Shared MESI
                        data_cache[index_decimal][i][3] = address
                        #increment respective stats
                        data_num_cache_hit += 1
                        break
                    elif data_cache[index_decimal][i][0] == tag and data_cache[index_decimal][i][2] == 0:
                        if print_message == 'y':
                            print("Read from L2 " + address)
                        tag_exists = True
                        data_num_cache_miss += 1
                        LRU = data_cache[index_decimal][i][1]
                        for ii in range(data_ways):
                            if data_cache[index_decimal][ii][1] != 'empty' and data_cache[index_decimal][ii][1] > LRU:
                                data_cache[index_decimal][ii][1] -= 1
                        data_cache[index_decimal][i][1] = 7
                        data_cache[index_decimal][i][2] = 1
                        data_cache[index_decimal][i][3] = address
                        break
                #if cache miss, check for empty ways, if any - save tag there
                #if no empty ways, check for invalid, if present, save to LRU invalid way
                #if no invalid, save to LRU way, update LRU (for all ways) and MESI (for specified way only)
                if not tag_exists:
                    if print_message == 'y':
                        print("Read from L2 " + address)                                                # Communication to L2. Refers to bullet 3 on PD.
                     
                    data_num_cache_miss += 1
                    for i in range(data_ways):
                        if data_cache[index_decimal][i][0] == 'empty':
                            data_cache[index_decimal][i][0] = tag
                            
                            for ii in range(data_ways):
                                if data_cache[index_decimal][ii][0] != tag:
                                    if data_cache[index_decimal][ii][1] != 'empty':
                                        data_cache[index_decimal][ii][1] -= 1
                            data_cache[index_decimal][i][1] = 7 #111 LRU
                            data_cache[index_decimal][i][2] = 2 #exclusive MESI
                            data_cache[index_decimal][i][3] = address
                            break
                        # for invalid checking
                        if i == 7:
                            invalid_found = False
                            LRU_Invalid = None
                            
                            for ii in range(data_ways):
                                if data_cache[index_decimal][ii][2] == 0:
                                    if LRU_Invalid == None or data_cache[index_decimal][LRU_Invalid][1] > data_cache[index_decimal][ii][1]:
                                        LRU_Invalid = ii
                                    invalid_found = True
                                    
                            if invalid_found:
                                data_cache[index_decimal][LRU_Invalid][0] = tag
                                
                                LRU = data_cache[index_decimal][LRU_Invalid][1]
                                for iii in range(data_ways):
                                    if data_cache[index_decimal][iii][1] > LRU:
                                        data_cache[index_decimal][iii][1] -= 1
                                data_cache[index_decimal][LRU_Invalid][1] = 7
                                data_cache[index_decimal][LRU_Invalid][2] = 2
                                data_cache[index_decimal][LRU_Invalid][3] = address
                            
                            else:   # Invalid_found = false
                                for ii in range(data_ways):
                                    if data_cache[index_decimal][ii][1] == 0:
                                        data_cache[index_decimal][ii][0] = tag
                                        LRU = data_cache[index_decimal][ii][1]
                                        for iii in range(data_ways):
                                            if data_cache[index_decimal][iii][1] > LRU:
                                                data_cache[index_decimal][iii][1] -= 1
                                        data_cache[index_decimal][ii][1] = 7
                                        data_cache[index_decimal][ii][2] = 2
                                        data_cache[index_decimal][ii][3] = address
                                        break
            #command 1 - write to data cache
            elif mode == 1:
                #increment respective stats
                data_num_cache_write += 1
                tag_exists = False
                for i in range(len(data_cache[index_decimal])):
                    #cache hit - tag present and NOT invalid
                    #update LRU (for all ways) and MESI (for specified way only)
                    if data_cache[index_decimal][i][0] == tag and data_cache[index_decimal][i][2] != 0:
                        tag_exists = True
                        data_num_cache_hit += 1
                        LRU = data_cache[index_decimal][i][1]
                        for ii in range(data_ways):
                            if data_cache[index_decimal][ii][1] != 'empty' and data_cache[index_decimal][ii][1] > LRU:
                                data_cache[index_decimal][ii][1] -= 1
                        data_cache[index_decimal][i][1] = 7
                        data_cache[index_decimal][i][2] = 3
                        data_cache[index_decimal][i][3] = address
                        break
                    #cache miss - if tag in cache but marked invalid, RFO from L2
                    #update LRU (for all ways) and MESI (for specified way only)
                    elif data_cache[index_decimal][i][0] == tag and data_cache[index_decimal][i][2] == 0:
                        if print_message == 'y':
                            print("Read for Ownership from L2 " + address)
                        tag_exists = True
                        data_num_cache_miss += 1
                        LRU = data_cache[index_decimal][i][1]
                        for ii in range(data_ways):
                            if data_cache[index_decimal][ii][1] != 'empty' and data_cache[index_decimal][ii][1] > LRU:
                                data_cache[index_decimal][ii][1] -= 1
                        data_cache[index_decimal][i][1] = 7
                        data_cache[index_decimal][i][2] = 3
                        data_cache[index_decimal][i][3] = address
                        break
                        
                #if cache miss, RFO from L2, check for empty ways, if any - write tag there
                #if no empty ways, check for invalid, if present, save to LRU invalid way
                #if no invalid, save to LRU way, update LRU (for all ways) and MESI (for specified way only)
                if not tag_exists:
                    data_num_cache_miss += 1
                    
                    for i in range(data_ways):
                        if data_cache[index_decimal][i][0] == 'empty':
                            data_cache[index_decimal][i][0] = tag
                            
                            if print_message == 'y':
                                print("Read for Ownership from L2 " + address)
                                print("Write to L2 " + address)
                                
                            for ii in range(data_ways):
                                if data_cache[index_decimal][ii][1] != 'empty' and data_cache[index_decimal][ii][0] != tag:
                                    data_cache[index_decimal][ii][1] -= 1
                                    
                            data_cache[index_decimal][i][1] = 7 #111 LRU
                            data_cache[index_decimal][i][2] = 3 #modified MESI
                            data_cache[index_decimal][i][3] = address
                            break
                        if i == 7:
                            invalid_found = False
                            LRU_Invalid = None
                            
                            for ii in range(data_ways):
                                if data_cache[index_decimal][ii][2] == 0:
                                    if LRU_Invalid == None or data_cache[index_decimal][LRU_Invalid][1] > data_cache[index_decimal][ii][1]:
                                        LRU_Invalid = ii
                                    invalid_found = True
                                    
                            if invalid_found:
                                data_cache[index_decimal][LRU_Invalid][0] = tag
                                
                                LRU = data_cache[index_decimal][LRU_Invalid][1]
                                for iii in range(data_ways):
                                    if data_cache[index_decimal][iii][1] > LRU:
                                        data_cache[index_decimal][iii][1] -= 1
                                data_cache[index_decimal][LRU_Invalid][1] = 7
                                data_cache[index_decimal][LRU_Invalid][2] = 3
                                data_cache[index_decimal][LRU_Invalid][3] = address
                            
                            else:   # Invalid_found = false
                                for ii in range(data_ways):
                                    if data_cache[index_decimal][ii][1] == 0:
                                        if print_message == 'y':
                                            print("Write to L2 " + data_cache[index_decimal][ii][3])
                                            print("Read for Ownership from L2 " + address)
                                            
                                        data_cache[index_decimal][ii][0] = tag
                                        LRU = data_cache[index_decimal][ii][1]
                                        for iii in range(data_ways):
                                            if data_cache[index_decimal][iii][1] > LRU:
                                                data_cache[index_decimal][iii][1] -= 1
                                        data_cache[index_decimal][ii][1] = 7
                                        data_cache[index_decimal][ii][2] = 3
                                        data_cache[index_decimal][ii][3] = address
                                        break
            #command 2 - read to instruction cache
            elif mode == 2:
                #increment respective stats
                inst_num_cache_read += 1
                tag_exists = False
                #cache hit - tag present and NOT invalid
                #update LRU (for all ways) and MESI (for specified way only)
                for i in range(len(instruction_cache[index_decimal])):
                    # cache hit
                    if instruction_cache[index_decimal][i][0] == tag and data_cache[index_decimal][i][2] != 0:
                        tag_exists = True
                        LRU = instruction_cache[index_decimal][i][1]
                        for ii in range(inst_ways):
                            if instruction_cache[index_decimal][ii][1] != "empty":                                                                                 # Added: Compares the empty string first before comparing LRUs
                                if instruction_cache[index_decimal][ii][1] > LRU:
                                    instruction_cache[index_decimal][ii][1] -= 1
                        instruction_cache[index_decimal][i][1] = 3
                        instruction_cache[index_decimal][i][2] = 1     # Shared MESI
                        instruction_cache[index_decimal][i][3] = address
                            
                        inst_num_cache_hit += 1
                        break
                    elif data_cache[index_decimal][i][0] == tag and data_cache[index_decimal][i][2] == 0:
                        if print_message == 'y':
                            print("Read from L2 " + address)
                        tag_exists = True
                        data_num_cache_miss += 1
                        LRU = data_cache[index_decimal][i][1]
                        for ii in range(data_ways):
                            if data_cache[index_decimal][ii][1] != 'empty' and data_cache[index_decimal][ii][1] > LRU:
                                data_cache[index_decimal][ii][1] -= 1
                        data_cache[index_decimal][i][1] = 7
                        data_cache[index_decimal][i][2] = 1
                        data_cache[index_decimal][i][3] = address
                        break
                #if cache miss, check for empty ways, if any - write tag there
                #if no empty ways, check for invalid, if present, save to LRU invalid way
                #if no invalid, save to LRU way, update LRU (for all ways) and MESI (for specified way only)
                if not tag_exists:
                    if print_message == 'y':
                        print("Read from L2 " + address)                                                # Communication to L2. Refers to bullet 3 on PD.
                     
                    inst_num_cache_miss += 1
                    for i in range(inst_ways):
                        if instruction_cache[index_decimal][i][0] == 'empty':
                            instruction_cache[index_decimal][i][0] = tag
                            
                            for ii in range(inst_ways):
                                if instruction_cache[index_decimal][ii][0] != tag:
                                    if instruction_cache[index_decimal][ii][1] != 'empty':
                                        instruction_cache[index_decimal][ii][1] -= 1
                            instruction_cache[index_decimal][i][1] = 3 #11 LRU
                            instruction_cache[index_decimal][i][2] = 2 #exclusive MESI
                            instruction_cache[index_decimal][i][3] = address
                            break
                        # for invalid checking
                        if i == 3:
                            invalid_found = False
                            LRU_Invalid = None
                            
                            for ii in range(inst_ways):
                                if instruction_cache[index_decimal][ii][2] == 0:
                                    if LRU_Invalid == None or instruction_cache[index_decimal][LRU_Invalid][1] > instruction_cache[index_decimal][ii][1]:
                                        LRU_Invalid = ii
                                    invalid_found = True
                                    
                            if invalid_found:
                                instruction_cache[index_decimal][LRU_Invalid][0] = tag
                                
                                LRU = instruction_cache[index_decimal][LRU_Invalid][1]
                                for iii in range(inst_ways):
                                    if instruction_cache[index_decimal][iii][1] > LRU:
                                        instruction_cache[index_decimal][iii][1] -= 1
                                instruction_cache[index_decimal][LRU_Invalid][1] = 3
                                instruction_cache[index_decimal][LRU_Invalid][2] = 2
                                instruction_cache[index_decimal][LRU_Invalid][3] = address
                            
                            else:   # Invalid_found = false
                                for ii in range(inst_ways):
                                    if instruction_cache[index_decimal][ii][1] == 0:
                                        instruction_cache[index_decimal][ii][0] = tag
                                        LRU = instruction_cache[index_decimal][ii][1]
                                        for iii in range(inst_ways):
                                            if instruction_cache[index_decimal][iii][1] > LRU:
                                                instruction_cache[index_decimal][iii][1] -= 1
                                        instruction_cache[index_decimal][ii][1] = 3
                                        instruction_cache[index_decimal][ii][2] = 2
                                        instruction_cache[index_decimal][ii][3] = address
                                        break
            #command 3 - invalidate specified set/way/tag
            elif mode == 3:
                #set MESI for specified set/tag to Invalid, update LRU (for all ways)
                for i in range(data_ways):
                    if data_cache[index_decimal][i][0] == tag:
                        # change the MESI Protocol to the Invalid state
                        LRU = data_cache[index_decimal][i][1]
                        for iii in range(data_ways):
                            if data_cache[index_decimal][iii][1] != 'empty' and data_cache[index_decimal][iii][1] > LRU:
                                data_cache[index_decimal][iii][1] -= 1
                        data_cache[index_decimal][i][1] = 7
                        data_cache[index_decimal][i][2] = 0
                        break
            #command 4 - pass specified set/way/tag to L2 in respnse to RFO
            #from another processor and set MESI for specified set/way/tag to invalid
            elif mode == 4:
                #pass specified set/tag to L2, set MESI for specified set/tag to Invalid, update LRU (for all ways)
                for i in range(data_ways):
                    if data_cache[index_decimal][i][0] == tag:
                        if print_message == 'y':
                            print("Return data to L2 " + address)                                # Communication to L2. Refers to bullet 1 on PD.
                        
                        # change the MESI Protocol to the Invalid state
                        LRU = data_cache[index_decimal][i][1]
                        for iii in range(data_ways):
                            if data_cache[index_decimal][iii][1] != 'empty' and data_cache[index_decimal][iii][1] > LRU:
                                data_cache[index_decimal][iii][1] -= 1
                        data_cache[index_decimal][i][1] = 7
                        data_cache[index_decimal][i][2] = 0
                        break
            #command 8 - set MESI for all sets/ways/tags to invalid
            elif mode == 8:
                #set MESI for all sets/ways to invalid and reset stats
                for i in range(len(data_cache)):
                    for ii in range(data_ways):
                        data_cache[i][ii][2] = 0
                for i in range(len(instruction_cache)):
                    for ii in range(inst_ways):
                        instruction_cache[i][ii][2] = 0
                data_num_cache_read = 0
                data_num_cache_write = 0
                data_num_cache_hit = 0
                data_num_cache_miss = 0
                data_cache_hit_ratio = 0.0

                inst_num_cache_read = 0
                inst_num_cache_hit = 0
                inst_num_cache_miss = 0
                inst_cache_hit_ratio = 0.0
            #command 9 - print cache contents and stats
            elif mode == 9:
                
                for index_parse in range(capacity):
                    for i in range(data_ways):
                        if data_cache[index_parse][i][0] != 'empty':
                            print ("\n\nPrinting Data Cache... \n")
                            print("{:<12} {:<8} {:<10} {:<6} {:<4}".format("Index/Set", "Way", "Tag", "LRU", "MESI"))
                            for way in range(data_ways):
                                if data_cache[index_parse][way][0] != 'empty':
                                    loc_index = hex(index_parse)[2:].zfill(4).upper()
                                    loc_tag = hex(int(data_cache[index_parse][way][0],2))[2:].zfill(3).upper()
                                    loc_LRU = bin(data_cache[index_parse][way][1])[2:].zfill(3)
                                    loc_MESI = convert_mesi(data_cache[index_parse][way][2])
                                    print("0x{:<10} {:<8} 0x{:<8} {:<6} {:<4}".format(loc_index, way+1, loc_tag,  loc_LRU, loc_MESI))
                                else:
                                    loc_index = hex(index_parse)[2:].zfill(4).upper()
                                    loc_tag = 'EMPTY'
                                    loc_LRU = '000'
                                    loc_MESI = 'I'
                                    print("0x{:<10} {:<8} {:<10} {:<6} {:<4}".format(loc_index, way+1, loc_tag,  loc_LRU, loc_MESI))
                            break
                
                for index_parse in range(capacity):
                    for i in range(inst_ways):
                        if instruction_cache[index_parse][i][0] != 'empty':
                            print ("\n\nPrinting Instruction Cache... \n")
                            print("{:<12} {:<8} {:<10} {:<6} {:<4}".format("Index/Set", "Way", "Tag", "LRU", "MESI"))
                            for way in range(inst_ways):
                                if instruction_cache[index_parse][way][0] != 'empty':
                                    loc_index = hex(index_parse)[2:].zfill(4).upper()
                                    loc_tag = hex(int(instruction_cache[index_parse][way][0],2))[2:].zfill(3).upper()
                                    loc_LRU = bin(instruction_cache[index_parse][way][1])[2:].zfill(2)
                                    loc_MESI = convert_mesi(instruction_cache[index_parse][way][2])
                                    print("0x{:<10} {:<8} 0x{:<8} {:<7} {:<4}".format(loc_index, way+1, loc_tag,  loc_LRU, loc_MESI))
                                else:
                                    loc_index = hex(index_parse)[2:].zfill(4).upper()
                                    loc_tag = 'EMPTY'
                                    loc_LRU = '000'
                                    loc_MESI = 'I'
                                    print("0x{:<10} {:<8} {:<10} {:<6} {:<4}".format(loc_index, way+1, loc_tag,  loc_LRU, loc_MESI))
                            break
                
                if not data_num_cache_hit and not data_num_cache_miss:
                    data_cache_hit_ratio = 0.0
                else:
                    data_cache_hit_ratio = 100.0 * data_num_cache_hit / (data_num_cache_hit + data_num_cache_miss)
                 
                if not inst_num_cache_hit and not inst_num_cache_miss:
                    inst_cache_hit_ratio = 0.0
                else:
                    inst_cache_hit_ratio = 100.0 * inst_num_cache_hit / (inst_num_cache_hit + inst_num_cache_miss)
                 
                print('\nData Cache Stats:')
                print("{:<9} {:<9} {:<9} {:<9} {:<9}".format("Reads", "Writes", "Hits", "Misses", "Hit Ratio"))
                print("{:<9} {:<9} {:<9} {:<11} {:5.2f} %".format(data_num_cache_read, data_num_cache_write, data_num_cache_hit,  data_num_cache_miss, data_cache_hit_ratio))
                print('\nInstruction Cache Stats:')
                print("{:<9} {:<9} {:<9} {:<9}".format("Reads", "Hits", "Misses", "Hit Ratio"))
                print("{:<9} {:<9} {:<11} {:5.2f} %".format(inst_num_cache_read, inst_num_cache_hit,  inst_num_cache_miss, inst_cache_hit_ratio))
                proceed = input("Continue?\n")

            elif mode == None:
                continue
            else:
                print("Invalid mode entered for given command line: " + data)
                continue
                
        print("End of file reached")
        if input("Exit program? y/n\n") == 'y':
            break
        