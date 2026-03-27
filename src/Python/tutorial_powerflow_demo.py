#------------------------------------------------------------------------------------------------
# Python classes (power-systems flavored tutorial)
#------------------------------------------------------------------------------------------------

"""
Topics touched here:
  - Class vs instance attributes
  - __str__ vs __repr__
  - @property (getter / setter, computed values)
  - Dunder methods: __len__, __getitem__, __iter__, __add__, __repr__
  - Inheritance: super(), method overrides, polymorphism
  - abc.ABC and @abstractmethod (PowerFlowSolver + concrete solvers)
  - dataclasses: @dataclass, field(default_factory=...)
  - Context managers: __enter__ / __exit__ (e.g. timing with SolverTimer)
  - Instance methods, @classmethod, @staticmethod
  - Entry point: if __name__ == "__main__"

Read top-to-bottom: each class first, then its demo_*() function shows how to use it.
"""

from __future__ import annotations

import math
import time

import numpy as np
from abc import ABC, abstractmethod
from collections.abc import Iterator
from dataclasses import dataclass, field
from typing import Optional

#--------------------------------------------------------
# Bus class
#--------------------------------------------------------

class Bus:
    """
    A network bus (node). In load flow, buses are often typed as PQ, PV, or slack
    (reference for angle and sometimes voltage).

    Conceptual one-line (not to scale):

        Bus k                Bus m
          o--------------------o
        (PV)                  (PQ)
          |                     |
       injection              load

    Slack bus: fixes voltage angle (and often |V|); other buses solve P, Q, or |V|.

    Voltage magnitude is stored in per unit; @property lets us validate assignments
    while callers still use bus.voltage = x.
    """

    # Acceptable per-unit voltage band for this tutorial (typical screening range).
    PU_VOLTAGE_MIN: float = 0.5
    PU_VOLTAGE_MAX: float = 1.5

    # Class variable: one shared integer for every Bus instance.
    # Increment it via the class name (Bus.count), not self.count, so it is clearly
    # not "per object" state. (Using self.count += 1 can accidentally shadow the
    # class variable on the instance--a common gotcha.)
    count: int = 0

    def __init__(
        self,
        bus_id: int,
        voltage: float = 1.0,
        bus_type: str = "PQ",
        nominal_line_kv: float = 345.0,
    ) -> None:
        """
        Initializes a Bus object.

        Args:
            bus_id: The ID of the bus (integer).
            voltage: Line-to-line voltage magnitude in per unit on nominal_line_kv (float). Default is 1.0 per unit.
            bus_type: The type of the bus (str). Default is "PQ".
            nominal_line_kv: Line-to-line base voltage in kV for pu conversion (float). Default is 345.0 kV.
        """

        # initialize the bus_id attribute
        self.bus_id = bus_id
        # initialize the nominal_line_kv attribute- private attribute
        self._nominal_line_kv = nominal_line_kv
        # initialize the voltage attribute- private attribute
        self._voltage_pu = voltage
        # initialize the bus_type attribute
        self.bus_type = bus_type
        # increment the class variable count
        Bus.count += 1

    @property
    def voltage(self) -> float:
        """Per-unit line-to-line voltage magnitude on nominal_line_kv base."""
        return self._voltage_pu

    @voltage.setter
    def voltage(self, value: float) -> None:
        if not (self.PU_VOLTAGE_MIN <= value <= self.PU_VOLTAGE_MAX):
            raise ValueError(
                f"Voltage {value} pu out of range "
                f"[{self.PU_VOLTAGE_MIN}, {self.PU_VOLTAGE_MAX}]"
            )
        self._voltage_pu = value

    @property
    def line_voltage_kv(self) -> float:
        """Line-to-line voltage in kV: (pu) * (nominal base kV)."""
        return self._voltage_pu * self._nominal_line_kv

    def __repr__(self) -> str:
        """
        Returns the "official" string representation of the Bus object.

        Returns:
            A string usable by repr() and debuggers (str).
        """

        # __repr__: "official" string; repr(obj) and interactive shells use this.
        # Aim for something unambiguous (often looks like a constructor call).
        return (
            f"Bus(id={self.bus_id}, V={self.voltage:.3f} pu, "
            f"{self.line_voltage_kv:.1f} kV, type={self.bus_type})"
        )

    def __str__(self) -> str:
        """
        Returns the "user-friendly" string representation of the Bus object.

        Returns:
            A string for end users, e.g. from print() (str).
        """

        # __str__: user-facing; print(obj) calls str(obj), which prefers __str__.
        # If __str__ were missing, print would fall back to __repr__.
        return (
            f"Bus {self.bus_id}: {self.voltage:.3f} pu ({self.line_voltage_kv:.1f} kV) "
            f"{self.bus_type}"
        )


def demo_bus() -> None:
    """
    Demonstrates the Bus class, including __str__, __repr__, and @property.
    """

    print("Bus - class vs instance, __str__ vs __repr__, @property")
    print("-" * 56)

    # Keyword arguments (voltage=..., bus_type=...) are optional style for clarity.
    b1 = Bus(1, voltage=1.02, bus_type="PV")
    b2 = Bus(2)

    # f-strings call __str__ for {b1}; repr(b1) calls __repr__ explicitly
    print(f"b1 (str):  {b1}")
    print(f"b1 (repr): {repr(b1)}")
    print(f"b2:        {b2}")
    print(f"Bus.count: {Bus.count}   # class variable: two __init__ calls so far")


    print()
    print("@property: voltage (pu), line_voltage_kv (computed), setter validation")
    print(f"  b1.voltage = {b1.voltage} pu  ->  line_voltage_kv = {b1.line_voltage_kv:.1f} kV")

    # set the voltage to 1.05 pu
    b1.voltage = 1.05
    print(f"  after b1.voltage = 1.05: {b1.voltage} pu, {b1.line_voltage_kv:.1f} kV")
    # try to set the voltage to 2.0 pu
    try:
        b1.voltage = 2.0
    except ValueError as e:
        # print the error message
        print(f"  b1.voltage = 2.0 rejected: {e}")

#--------------------------------------------------------
# PowerLine class
#--------------------------------------------------------

class PowerLine:
    """
    A series branch between two buses (transmission line segment model).

    r and x are in per unit on the system base (typical in textbook problems).
    Ohms to pu: Z_pu = Z_ohm / Z_base, with Z_base = V_base**2 / S_base.

        from_bus                   to_bus
        o--------[ z = r + jx ]--------o
       (i)                              (j)

    Shunt charging / transformers are not modeled here (series branch only).
    """

    # Class attribute: default system base (MVA). All instances see the same value
    # unless you rebind the attribute on the class (see set_base_mva demo).
    base_mva: float = 100.0

    def __init__(
        self,
        from_bus: int,
        to_bus: int,
        r: float,
        x: float,
        rating_mw: float,
    ) -> None:
        """
        Initializes a PowerLine object.

        Args:
            from_bus: The ID of the from bus (integer).
            to_bus: The ID of the to bus (integer).
            r: Series resistance in per unit on system base (float).
            x: Series reactance in per unit on system base (float).
            rating_mw: Thermal or nameplate rating of the line in MW (float).
        """

        # Basic validation: keeps 1/z and loading_pct well-defined in the tutorial.
        if rating_mw <= 0:
            raise ValueError("rating_mw must be positive")

        # Instance attributes: each line has its own endpoints, impedance, rating.
        self.from_bus = from_bus
        self.to_bus = to_bus
        self.z = complex(r, x)
        if self.z == 0:
            raise ValueError("impedance z = r + jx must be non-zero (need 1/z)")
        self.y = 1.0 / self.z
        self.rating_mw = rating_mw

    # Instance method: first argument is always self (the instance)
    # Use for anything that needs this line's data (z, rating, bus IDs, ...).
    def loading_pct(self, flow_mw: float) -> float:
        """
        Computes apparent loading versus the line MW rating.

        Args:
            flow_mw: Active power flow magnitude on the line in MW (float).

        Returns:
            Loading as a percentage of rating (float).
        """
        return abs(flow_mw) / self.rating_mw * 100.0

    # Class method: first argument is cls (the class object)
    # Typical uses: alternative constructors, or changing class-level settings.
    @classmethod
    def from_impedance_string(
        cls,
        from_bus: int,
        to_bus: int,
        z_str: str,
        rating_mw: float,
    ) -> PowerLine:
        """
        Alternative constructor: parse series impedance from a string.

        Args:
            from_bus: The ID of the from bus (integer).
            to_bus: The ID of the to bus (integer).
            z_str: Impedance as a Python complex literal, e.g. '0.02+0.08j' (str).
            rating_mw: Thermal or nameplate rating of the line in MW (float).

        Returns:
            A new PowerLine instance.
        """
        z = complex(z_str)
        # cls(...) calls __init__ the same way PowerLine(...) would; use cls so
        # subclasses could override __init__ and still work.
        return cls(from_bus, to_bus, z.real, z.imag, rating_mw)

    @classmethod
    def set_base_mva(cls, base: float) -> None:
        """
        Sets the class-level system base in MVA.

        Args:
            base: New three-phase base apparent power in MVA (float).

        Returns:
            None.
        """
        if base <= 0:
            raise ValueError("base MVA must be positive")
        cls.base_mva = base

    # Static method: no self, no cls
    # Use for helpers that belong next to the class logically but do not need
    # instance or class data. Could be a module-level function; staticmethod groups it.
    @staticmethod
    def is_valid_impedance(r: float, x: float) -> bool:
        """
        Checks typical inductive-line sign pattern (non-negative R, positive X).

        Args:
            r: Series resistance in per unit (float).
            x: Series reactance in per unit (float).

        Returns:
            True if r >= 0 and x > 0, else False (bool).
        """
        return r >= 0.0 and x > 0.0

    def __repr__(self) -> str:
        """
        Returns the "official" string representation of the PowerLine object.

        Returns:
            A string usable by repr() and print() when __str__ is not defined (str).
        """
        # We only define __repr__; print(line) falls back to it when __str__ is absent.
        return (
            f"PowerLine({self.from_bus}->{self.to_bus}, z={self.z!s}, "
            f"{self.rating_mw} MW, base {self.base_mva} MVA)"
        )


def demo_power_line() -> None:
    """
    Demonstrates the PowerLine class, including instance methods, classmethods,
    staticmethods, and the shared class attribute base_mva.
    """
    print("PowerLine - instance / class / static methods")
    print("-" * 56)

    # Primary constructor: PowerLine(from_bus, to_bus, r, x, rating_mw)
    # create a line from bus 1 to bus 2 with resistance 0.01 pu, reactance 0.05 pu, and rating 200.0 MW
    line = PowerLine(from_bus=1, to_bus=2, r=0.01, x=0.05, rating_mw=200.0)

    # Alternative constructor: classmethod builds the same kind of object: PowerLine.from_impedance_string(from_bus, to_bus, z_str, rating_mw)
    # create a line from bus 2 to bus 3 with impedance 0.02+0.08j pu, and rating 150.0 MW
    line_alt = PowerLine.from_impedance_string(from_bus=2, to_bus=3, z_str="0.02+0.08j", rating_mw=150.0)

    # print the line using __str__ if defined, else __repr__
    print(f"{line}")
    print(f"{line_alt}")

    # Instance method: needs a specific line (self is bound as line)
    flow_mw = 180.0
    pct = line.loading_pct(flow_mw)
    print(
        f"Line {line.from_bus}-{line.to_bus}: |P| = {flow_mw} MW  ->  "
        f"{pct:.1f}% of {line.rating_mw} MW rating"
    )
    print()

    # Staticmethod: call on the class (usual)
    # You can also call line.is_valid_impedance(...) but that still does not pass self--Python just finds the function on the class.
    print("Static method (no self, no cls):")
    print(f"  is_valid_impedance(0.01, 0.05)  -> {PowerLine.is_valid_impedance(0.01, 0.05)}")
    print(f"  is_valid_impedance(0.01, -0.05) -> {PowerLine.is_valid_impedance(0.01, -0.05)}")
    print()

    # Classmethod set_base_mva mutates PowerLine.base_mva; all instances read the new value
    print("Class method changes shared base_mva:")
    print(f"  Before: PowerLine.base_mva = {PowerLine.base_mva}")
    print(f"  line.base_mva reads {line.base_mva}  (same class attribute)")
    print(f"  line_alt.base_mva reads {line_alt.base_mva}  (same class attribute)")
    # set the base_mva to 200.0 MVA
    PowerLine.set_base_mva(200.0)
    print(f"  After:  PowerLine.base_mva = {PowerLine.base_mva}")
    print(f"  line.base_mva reads {line.base_mva}  (same class attribute)")
    print(f"  line_alt.base_mva reads {line_alt.base_mva}  (same class attribute)")


#--------------------------------------------------------
# Asset classes (inheritance example)
#--------------------------------------------------------

class Asset:
    """
    Base class for simple grid-style financial assets (location, value, risk).

    Subclasses add technology-specific fields (wind, solar, etc.) and methods.
    """

    def __init__(
        self,
        asset_id: str,
        location: tuple[float, float],
        value_usd: float,
    ) -> None:
        """
        Initializes an Asset object.

        Args:
            asset_id: A string label for the asset (str).
            location: Approximate (latitude, longitude) in decimal degrees (tuple).
            value_usd: Replacement or book value in USD (float).
        """
        self.asset_id = asset_id
        self.location = location
        self.value_usd = value_usd

    def expected_annual_loss(
        self,
        damage_fraction: float,
        frequency: float,
    ) -> float:
        """
        Simple scalar risk model: value times damage fraction times event frequency.

        Args:
            damage_fraction: Fraction of value lost if an event occurs (float).
            frequency: Mean events per year (float).

        Returns:
            Expected annual loss in USD (float).
        """
        return self.value_usd * damage_fraction * frequency

    def describe(self) -> str:
        """
        Returns a short human-readable summary of the asset.

        Returns:
            A one-line description (str).
        """
        return f"Asset {self.asset_id} at {self.location}"


class WindTurbine(Asset):
    def __init__(
        self,
        asset_id: str,
        location: tuple[float, float],
        value_usd: float,
        capacity_mw: float,
        hub_height_m: float,
    ) -> None:
        """
        Initializes a WindTurbine object.

        Args:
            asset_id: A string label for the asset (str).
            location: Approximate (latitude, longitude) in decimal degrees (tuple).
            value_usd: Replacement or book value in USD (float).
            capacity_mw: Nameplate capacity in MW (float).
            hub_height_m: Hub height above ground in meters (float).
        """
        # call the parent class __init__ method to initialize the Asset object
        super().__init__(asset_id, location, value_usd)

        # initialize the WindTurbine-specific attributes
        self.capacity_mw = capacity_mw
        self.hub_height_m = hub_height_m

    def describe(self) -> str:
        """
        Overrides Asset.describe to include wind-specific capacity.

        Returns:
            A one-line description (str).
        """
        base = super().describe()
        return f"{base} | {self.capacity_mw} MW wind turbine"

    def wind_speed_at_hub(
        self,
        ref_speed: float,
        ref_height: float = 10.0,
    ) -> float:
        """
        Rough log-law extrapolation of wind speed to hub height (roughness z0 = 0.03 m).

        Args:
            ref_speed: Wind speed at reference height (float).
            ref_height: Reference height above ground in meters (float).

        Returns:
            Estimated wind speed at hub height (float).
        """
        z0 = 0.03
        return ref_speed * math.log(self.hub_height_m / z0) / math.log(ref_height / z0)


class SolarPanel(Asset):
    def __init__(
        self,
        asset_id: str,
        location: tuple[float, float],
        value_usd: float,
        capacity_mw: float,
        tilt_deg: float,
    ) -> None:
        """
        Initializes a SolarPanel object.

        Args:
            asset_id: A string label for the asset (str).
            location: Approximate (latitude, longitude) in decimal degrees (tuple).
            value_usd: Replacement or book value in USD (float).
            capacity_mw: Nameplate AC or DC capacity in MW (float).
            tilt_deg: Surface tilt from horizontal in degrees (float).
        """
        super().__init__(asset_id, location, value_usd)
        self.capacity_mw = capacity_mw
        self.tilt_deg = tilt_deg

    def describe(self) -> str:
        """
        Overrides Asset.describe to include solar PV capacity.

        Returns:
            A one-line description (str).
        """
        return f"{super().describe()} | {self.capacity_mw} MW solar PV"


def demo_asset() -> None:
    """
    Demonstrates inheritance, super(), overrides, and polymorphism (same call, different types).
    """
    print("Asset / WindTurbine / SolarPanel - inheritance and polymorphism")
    print("-" * 56)

    assets: list[Asset] = [
        WindTurbine("W-01", (28.4, -90.2), 8_000_000, 5.0, 100.0),
        SolarPanel("S-01", (33.1, -97.5), 3_000_000, 2.0, 25.0),
    ]

    # Polymorphism: each concrete class runs its own describe() implementation.
    for asset in assets:
        print(asset.describe())

    # Shared behavior from the base class (same signature on every Asset)
    w = assets[0]
    assert isinstance(w, WindTurbine)
    loss = w.expected_annual_loss(damage_fraction=0.05, frequency=0.02)
    print()
    print(f"Example expected_annual_loss on {w.asset_id}: ${loss:,.0f} / yr")
    print(f"Example wind_speed_at_hub (7 m/s at 10 m): {w.wind_speed_at_hub(7.0):.2f} m/s at hub")


#--------------------------------------------------------
# EventLossTable (dunder methods, sequence-style API)
#--------------------------------------------------------

class EventLossTable:
    """
    Stores stochastic event losses for CLIMADA-style risk analysis.

    Each row is (event_id, annual frequency, loss_usd). Implements a small
    sequence-like API via dunder methods so len(), indexing, iteration, and + work.
    """

    def __init__(self) -> None:
        """
        Initializes an empty EventLossTable.

        Args:
            None.
        """
        # List of (event_id, frequency per year, loss in USD).
        self.events: list[tuple[str, float, float]] = []

    def add_event(self, event_id: str, frequency: float, loss_usd: float) -> None:
        """
        Appends one event row to the table.

        Args:
            event_id: Identifier for the scenario (str).
            frequency: Mean occurrences per year (float).
            loss_usd: Loss if the event occurs, in USD (float).

        Returns:
            None.
        """
        self.events.append((event_id, frequency, loss_usd))

    def __len__(self) -> int:
        """Supports len(table)."""
        return len(self.events)

    def __getitem__(self, idx: int) -> tuple[str, float, float]:
        """Supports table[idx] indexing."""
        return self.events[idx]

    def __iter__(self) -> Iterator[tuple[str, float, float]]:
        """Supports for row in table (yields event tuples)."""
        return iter(self.events)

    def __add__(self, other: EventLossTable) -> EventLossTable:
        """
        Concatenates two tables into a new EventLossTable (rows in order: self, then other).

        Args:
            other: Another EventLossTable (EventLossTable).

        Returns:
            A new EventLossTable containing all rows from both operands.
        """
        if not isinstance(other, EventLossTable):
            return NotImplemented
        combined = EventLossTable()
        combined.events = self.events + other.events
        return combined

    def __repr__(self) -> str:
        """
        Returns the "official" string representation of the EventLossTable.

        Returns:
            A short summary string (str).
        """
        return f"EventLossTable({len(self)} events)"

    @property
    def expected_annual_impact(self) -> float:
        """
        Expected annual loss: sum over events of frequency * loss_usd.

        Returns:
            Expected annual impact in USD (float).
        """
        return sum(freq * loss for _, freq, loss in self.events)


def demo_event_loss_table() -> None:
    """
    Demonstrates EventLossTable: dunder methods, indexing, iteration, +, and @property.
    """
    print("EventLossTable - __len__, __getitem__, __iter__, __add__, @property")
    print("-" * 56)

    # create a new event loss table
    event_loss_table_1 = EventLossTable()
    event_loss_table_1.add_event("E001", 0.008, 3_360_000)
    event_loss_table_1.add_event("E002", 0.002, 6_100_000)

    # print the length of the event loss table
    print(f"len(event_loss_table_1) = {len(event_loss_table_1)}")
    # print the first event in the event loss table
    print(f"event_loss_table_1[0] = {event_loss_table_1[0]}")
    # print the expected annual impact of the event loss table
    print(f"event_loss_table_1.expected_annual_impact = ${event_loss_table_1.expected_annual_impact:,.0f}")
    print()

    # print the events in the event loss table
    for event_id, freq, loss in event_loss_table_1:
        print(f"  {event_id}: freq={freq}, loss=${loss:,.0f}")

    # create a new event loss table
    event_loss_table_2 = EventLossTable()
    event_loss_table_2.add_event("E003", 0.001, 1_000_000)
    # combine the two event loss tables
    combined = event_loss_table_1 + event_loss_table_2
    # print the combined event loss table
    print()
    # print the expected annual impact of the combined event loss table
    print(f"event_loss_table_1 + event_loss_table_2: {combined!r}, expected_annual_impact = ${combined.expected_annual_impact:,.0f}")


#--------------------------------------------------------
# SolverTimer (context manager)
#--------------------------------------------------------


class SolverTimer:
    """
    Wall-time stopwatch for solver code paths.

    Use as: with SolverTimer() as t: ... then read t.elapsed_ms after the block.

    elapsed_ms works inside the block (including on return from inside with) because it
    uses live time until __exit__ freezes the final value.
    """

    def __enter__(self) -> SolverTimer:
        self.start = time.perf_counter()
        self._elapsed_ms_final: float | None = None
        return self

    def __exit__(self, exc_type: object, exc_val: object, exc_tb: object) -> bool:
        self._elapsed_ms_final = (time.perf_counter() - self.start) * 1000.0
        return False

    @property
    def elapsed_ms(self) -> float:
        if self._elapsed_ms_final is not None:
            return self._elapsed_ms_final
        return (time.perf_counter() - self.start) * 1000.0


#--------------------------------------------------------
# StudyConfig / PowerFlowResult (dataclasses)
#--------------------------------------------------------
#
# @dataclass auto-generates __init__, __repr__, __eq__, and more (see dataclasses docs).
# Use field(default_factory=...) for mutable defaults (lists, dicts), never [] or {} as defaults.


@dataclass
class StudyConfig:
    """
    Parameters for a power-flow or contingency study (illustrative container).
    """

    n_buses: int
    base_mva: float = 100.0
    tolerance: float = 1e-6
    max_iterations: int = 50
    solver: str = "newton_raphson"
    contingencies: list[str] = field(default_factory=list)


@dataclass
class PowerFlowResult:
    """
    Outputs from a solved power flow (illustrative container).
    """

    converged: bool
    n_iterations: int
    bus_voltages: list[float]
    bus_angles: list[float]
    line_flows: dict[tuple[int, int], float]
    residual: float
    solve_time_ms: float
    warning: Optional[str] = None

    @property
    def max_voltage(self) -> float:
        return max(self.bus_voltages)

    @property
    def min_voltage(self) -> float:
        return min(self.bus_voltages)

    def has_voltage_violations(self, vmin: float = 0.95, vmax: float = 1.05) -> bool:
        return any(v < vmin or v > vmax for v in self.bus_voltages)


def demo_dataclass() -> None:
    """Demonstrates @dataclass, field(default_factory=...), and PowerFlowResult helpers."""
    print("StudyConfig / PowerFlowResult - dataclasses")
    print("-" * 56)

    config = StudyConfig(n_buses=100, tolerance=1e-8)
    print(f"{config!r}")
    print()

    result = PowerFlowResult(
        converged=True,
        n_iterations=4,
        bus_voltages=[1.0, 0.94, 1.02],
        bus_angles=[0.0, -2.1, 1.5],
        line_flows={(1, 2): 45.0, (2, 3): -12.0},
        residual=3e-7,
        solve_time_ms=12.3,
    )
    print(f"{result!r}")
    print(f"  max_voltage = {result.max_voltage} pu, min_voltage = {result.min_voltage} pu")
    print(f"  has_voltage_violations(0.95, 1.05) = {result.has_voltage_violations()}")


#--------------------------------------------------------
# NetworkModel / PowerFlowSolver (numpy + SolverTimer)
#--------------------------------------------------------

@dataclass
class NetworkModel:
    """
    Minimal network container for the PowerFlowSolver API (tutorial only).

    Bus indices are 0 .. n_buses-1. ``lines`` holds undirected edges ``(i, j)``.

    Conceptual 2-bus topology for the demos (not to scale)::

        Bus 0                   Bus 1
          o-----------------------o
        index 0                 index 1
              edge (0, 1) in ``lines``

    Toy Newton: ``_state`` and ``_target`` are length-2 arrays (not one value per bus);
    residual is ``state - target`` with identity Jacobian (see NewtonRaphsonSolver).

    Toy DC: solve ``B * theta = P`` (pu); ``B`` and ``P`` are fixed for ``n_buses == 2``
    only (see build_susceptance_matrix / power_injections). Angles live in ``_angles``.

    ``is_connected()`` builds an adjacency list from ``lines`` and DFS-es from bus 0;
    all buses must be reachable or the graph is treated as islanded.
    """

    n_buses: int
    buses: list[int] = field(default_factory=list)
    lines: list[tuple[int, int]] = field(default_factory=list)
    _state: np.ndarray = field(
        default_factory=lambda: np.zeros(2, dtype=float),
        repr=False,
    )
    _target: np.ndarray = field(
        default_factory=lambda: np.ones(2, dtype=float),
        repr=False,
    )
    _angles: np.ndarray | None = field(default=None, repr=False)

    def is_connected(self) -> bool:
        """
        Returns whether every bus lies in one connected component (via ``lines``).

        Uses depth-first search from bus 0. Edges outside ``0 .. n_buses-1`` are ignored.

        Returns:
            True if ``n_buses <= 1``, or if all buses are reachable from bus 0; else False (bool).
        """
        if self.n_buses <= 1:
            return True
        if not self.lines:
            return False
        adj: list[list[int]] = [[] for _ in range(self.n_buses)]
        for a, b in self.lines:
            if 0 <= a < self.n_buses and 0 <= b < self.n_buses:
                adj[a].append(b)
                adj[b].append(a)
        seen: set[int] = {0}
        stack = [0]
        while stack:
            u = stack.pop()
            for v in adj[u]:
                if v not in seen:
                    seen.add(v)
                    stack.append(v)
        return len(seen) == self.n_buses

    def build_susceptance_matrix(self) -> np.ndarray:
        """
        Returns the toy 2-bus DC susceptance matrix ``B`` (dense, tutorial constants).

        Returns:
            A ``(2, 2)`` float ndarray (np.ndarray).

        Raises:
            ValueError: If ``n_buses != 2``.
        """
        if self.n_buses != 2:
            raise ValueError("build_susceptance_matrix: tutorial supports n_buses == 2 only")
        return np.array([[2.0, -1.0], [-1.0, 2.0]], dtype=float)

    def power_injections(self) -> np.ndarray:
        """
        Returns net DC active power injections ``P`` (per unit) for the toy 2-bus case.

        Returns:
            A length-2 float ndarray ``[0.5, -0.5]`` (np.ndarray).

        Raises:
            ValueError: If ``n_buses != 2``.
        """
        if self.n_buses != 2:
            raise ValueError("power_injections: tutorial supports n_buses == 2 only")
        return np.array([0.5, -0.5], dtype=float)

    def set_angles(self, angles: np.ndarray) -> None:
        """
        Stores DC bus angles (radians) after a DC solve.

        Args:
            angles: Bus angle vector (sequence or ndarray); copied as float64 (np.ndarray).

        Returns:
            None.
        """
        self._angles = np.asarray(angles, dtype=float).copy()

    @property
    def angles(self) -> np.ndarray | None:
        """DC angles (rad) after set_angles; None if no DC solve has run yet."""
        return self._angles


class PowerFlowSolver(ABC):
    """
    Abstract base for power-flow-style solvers used with NetworkModel.

    Callers use ``run(network)``, which rejects disconnected graphs, then delegates to
    ``_solve``. Concrete subclasses implement ``_solve`` (Newton loop, single DC solve, etc.).
    """

    def __init__(self, tol: float = 1e-6, max_iter: int = 50) -> None:
        """
        Initializes solver settings shared by concrete solvers.

        Args:
            tol: Convergence threshold on the residual infinity-norm for iterative solvers (float).
            max_iter: Maximum Newton (or outer) iterations (int).
        """
        self.tol = tol
        self.max_iter = max_iter

    def run(self, network: NetworkModel) -> dict[str, object]:
        """
        Runs connectivity checks, then the concrete ``_solve`` implementation.

        Args:
            network: The network model (NetworkModel).

        Returns:
            Result dict with keys such as ``converged`` (bool), ``iters`` (int), ``ms`` (float).

        Raises:
            ValueError: If the network is not connected (islands).
        """
        if not network.is_connected():
            raise ValueError("Network has islands - power flow will fail")
        return self._solve(network)

    @abstractmethod
    def _solve(self, network: NetworkModel) -> dict[str, object]:
        """
        Performs the actual solve; must be implemented by each concrete solver.

        Args:
            network: The network model (NetworkModel).

        Returns:
            A dictionary with at least ``converged``, ``iters``, and ``ms`` (wall time in ms).
        """
        ...


class NewtonRaphsonSolver(PowerFlowSolver):
    """
    Newton-style correction: each iteration solves ``J dx = -r`` with numpy.

    This tutorial uses a 2x2 identity Jacobian and residual ``network._state - network._target``
    (not a real Y-bus). Production tools use sparse Jacobians and factorization instead.
    """

    def _solve(self, network: NetworkModel) -> dict[str, object]:
        """
        Iterates until ``max(abs(residual)) < tol`` or ``max_iter`` is reached.

        Args:
            network: The network model whose ``_state`` is updated (NetworkModel).

        Returns:
            Dict with ``converged`` (bool), ``iters`` (int), ``ms`` (float, from SolverTimer).
        """
        with SolverTimer() as t:
            for i in range(self.max_iter):
                residual = self._residual(network)
                if float(np.max(np.abs(residual))) < self.tol:
                    return {"converged": True, "iters": i, "ms": t.elapsed_ms}
                jacobian = self._jacobian(network)
                dx = np.linalg.solve(jacobian, -residual)
                self._update(network, dx)
            return {"converged": False, "iters": self.max_iter, "ms": t.elapsed_ms}

    def _residual(self, network: NetworkModel) -> np.ndarray:
        """
        Toy residual ``f(x) = x - x_star`` as a length-2 vector.

        Args:
            network: Model providing ``_state`` and ``_target`` (NetworkModel).

        Returns:
            Residual ndarray (np.ndarray).
        """
        return network._state - network._target

    def _jacobian(self, network: NetworkModel) -> np.ndarray:
        """
        Returns the 2x2 identity (toy Jacobian for this tutorial).

        Args:
            network: Unused; kept for symmetry with real Jacobian(network) APIs (NetworkModel).

        Returns:
            ``numpy.eye(2)`` (np.ndarray).
        """
        return np.eye(2, dtype=float)

    def _update(self, network: NetworkModel, dx: np.ndarray) -> None:
        """
        Applies the Newton correction ``x <- x + dx``.

        Args:
            network: Model whose ``_state`` is updated (NetworkModel).
            dx: Correction vector (np.ndarray).

        Returns:
            None.
        """
        network._state = network._state + dx


class DCPowerFlowSolver(PowerFlowSolver):
    """
    DC power-flow approximation: one linear solve ``B * theta = P`` (numpy).

    ``B`` and ``P`` come from ``network`` (toy 2-bus values in this file). Angles are
    stored via ``network.set_angles``. Wall time is measured with SolverTimer.
    """

    def _solve(self, network: NetworkModel) -> dict[str, object]:
        """
        Solves ``theta = B^{-1} P`` and writes angles to the network.

        Args:
            network: Model providing ``build_susceptance_matrix`` and ``power_injections`` (NetworkModel).

        Returns:
            Dict with ``converged`` True, ``iters`` 1, and ``ms`` (float, from SolverTimer).
        """
        with SolverTimer() as t:
            b_matrix = network.build_susceptance_matrix()
            p_vec = network.power_injections()
            angles = np.linalg.solve(b_matrix, p_vec)
            network.set_angles(angles)
            return {"converged": True, "iters": 1, "ms": t.elapsed_ms}


def demo_solver() -> None:
    """
    Demonstrates PowerFlowSolver (ABC), SolverTimer, NetworkModel, NewtonRaphsonSolver,
    DCPowerFlowSolver, and rejection of a disconnected (islanded) network.
    """
    print("Demonstrating PowerFlowSolver (ABC), SolverTimer, NetworkModel, Newton, DC")
    print("-" * 56)

    # demonstrate the SolverTimer context manager - empty loop to measure the time it takes to execute
    with SolverTimer() as t:
        _ = sum(range(10_000))  # tiny CPU work
    print(f"SolverTimer (empty loop): {t.elapsed_ms:.3f} ms")
    print()

    # demonstrate the PowerFlowSolver abstract base class - should raise a TypeError
    try:
        PowerFlowSolver()
    except TypeError as e:
        print(f"PowerFlowSolver() raises as expected: {str(e).rstrip()}")
    print()

    # demonstrate the NewtonRaphsonSolver concrete solver - should converge in 20 iterations
    network = NetworkModel(n_buses=2, lines=[(0, 1)])
    nr = NewtonRaphsonSolver(tol=1e-9, max_iter=20)
    result = nr.run(network)
    print(
        f"NewtonRaphsonSolver.run(network): converged={result['converged']}, "
        f"iters={result['iters']}, time={result['ms']:.3f} ms"
    )
    print(f"  network._state = {network._state.tolist()}")
    print()

    # demonstrate the DCPowerFlowSolver concrete solver - should converge in 1 iteration
    network_dc = NetworkModel(n_buses=2, lines=[(0, 1)])
    dc = DCPowerFlowSolver()
    dc_result = dc.run(network_dc)
    print(
        f"DCPowerFlowSolver.run(network): converged={dc_result['converged']}, "
        f"iters={dc_result['iters']}, time={dc_result['ms']:.3f} ms"
    )
    ang = network_dc.angles
    print(f"  angles (rad) = {ang.tolist() if ang is not None else None}")
    print()

    # demonstrate the rejection of a disconnected (islanded) network - should raise a ValueError
    network_island = NetworkModel(n_buses=2, lines=[])
    try:
        NewtonRaphsonSolver().run(network_island)
    except ValueError as e:
        print(f"disconnected network: {str(e).rstrip()}")


#--------------------------------------------------------
# Main function
#--------------------------------------------------------

def main() -> None:
    """
    Runs the tutorial demos: dataclasses, Bus, PowerLine, Asset, EventLossTable,
    PowerFlowSolver (ABC), SolverTimer, NewtonRaphsonSolver, DCPowerFlowSolver.
    """
    print("Demonstrating Python classes - tutorial")
    print("=" * 56)
    print()

    # demonstrate the dataclasses
    demo_dataclass()
    print()
    # demonstrate the Bus class
    demo_bus()
    print()
    # demonstrate the PowerLine class
    demo_power_line()
    print()
    # demonstrate the Asset class
    demo_asset()
    print()
    # demonstrate the EventLossTable class
    demo_event_loss_table()
    print()
    # demonstrate the PowerFlowSolver (ABC), SolverTimer, NewtonRaphsonSolver, DCPowerFlowSolver
    demo_solver()


# Only run demos when this file is executed directly, not when it is imported.
# That way:  python tutorial_python_classes.py  runs main(), but
#   import tutorial_python_classes   does not print anything by default.
if __name__ == "__main__":
    main()

