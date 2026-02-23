# A simple rock-paper-scissor game implemented in Python

class Participant:
    def __init__(self, name):
        self.name = name
        self.points = 0
        self.choice = ""

    def choose(self):
        self.choice = input("{name}, select rock, paper, or scissor: ".format(name = self.name))
        print("{name} selects {choice}".format(name=self.name, choice = self.choice))

    def toNumericalChoice(self):
        switcher = {
            "rock": 0,
            "paper": 1,
            "scissor": 2
        }
        return switcher[self.choice]

    def incrementPoint(self):
        self.points += 1


class GameRound:
    def __init__(self, p1, p2):
        # The following matrix provides the rule for rock, paper, scissors:
        # The idea of using a matrix is to express what combination wins
        # over other combinations. A winning move gets a 1, a draw gets a 0,
        # and a losing move gets a -1.
        self.rules = [
            [0, -1, 1],
            [1, 0, -1],
            [-1, 1, 0]
        ]

        p1.choose()
        p2.choose()

        result = self.compareChoices(p1, p2)
        print(
            "Round resulted in a {result}".format(result = self.getResultAsString(result))
        )

        if result > 0:
           p1.incrementPoint()
        elif result < 0:
           p2.incrementPoint()

    def compareChoices(self, p1, p2):
        return self.rules[p1.toNumericalChoice()][p2.toNumericalChoice()]

    def getResultAsString(self, result):
        res = {
            0: "draw",
            1: "win",
            -1: "loss"
        }
        return res[result]


class Game:
    def __init__(self):
        self.end_game = False
        self.participant = Participant("Spock")
        self.second_participant = Participant("Kirk")

    def start(self):
        while not self.end_game:
            GameRound(self.participant, self.second_participant)
            self.checkEndCondition()

    def checkEndCondition(self):
        answer = input("Continue game y/n: ")
        if answer == 'y':
            GameRound(self.participant, self.second_participant)
            self.checkEndCondition()
        else:
            print(
                "Game ended, {p1name} has {p1points}, and {p2name} has {p2points}"\
                .format(p1name = self.participant.name, p1points= self.participant.points,\
                p2name=self.second_participant.name, p2points=self.second_participant.points)
            )

            self.determineWinner()
            self.endGame = True

    def determineWinner(self):
        result_string = "It's a Draw"

        if self.participant.points > self.second_participant.points:
            result_string = "Winner is {name}".format(name=self.participant.name)
        elif self.participant.points < self.second_participant.points:
            result_string = "Winner is {name}".format(name=self.second_participant.name)

        print(result_string)


game = Game()
game.start()

