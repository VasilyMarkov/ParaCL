import random

grammar = {
    "<E>": ["<E> + <E>", "<E> * <E>", "<NUMBER>", "<ASSIGN>", "<ID>"],
    "<ID>": ["var1", "var2", "var3"],
    "<NUMBER>": list(range(5)),  
    "<ASSIGN>": ["<ID_NOT_LAST> = <E>"],
    "<ID_NOT_LAST>": []  
}

last_id = None
for id in grammar["<ID>"]:
    if id != last_id:
        grammar["<ID_NOT_LAST>"].append(id)


def generate_phrase(phrase, grammar, last_id=None, depth=0, max_depth=5):
   if depth > max_depth:
        return phrase  
   if phrase in grammar:
      choices = grammar[phrase]
      if phrase == "<ID_NOT_LAST>" and last_id in choices:
         choices.remove(last_id)
      choice = random.choice(choices)
      if isinstance(choice, str):
         return " ".join(generate_phrase(part, grammar, last_id if part != "<ID_NOT_LAST>" else id, depth+1, max_depth) for part in choice.split())
      else:
         return str(choice)
   else:
      return phrase  

print(generate_phrase("<E>", grammar))