import random

def generate_test(num_ops, max_value):
    test = []
    for _ in range(num_ops):
        # Reinitialize the random number generator for each set
        random.seed()
        set_length = random.randint(1, 2)  # Generate a random set length between 1 and 2
        new_list = [str(random.randint(1, max_value)) for _ in range(set_length)]  # Generate random values between 1 and max_value
        test.append(new_list)

    with open('half_million.txt', 'w') as file:
        for new_list in test:
            file.write('{' + ', '.join(new_list) + '}' + '\n')

generate_test(500_000, 100)